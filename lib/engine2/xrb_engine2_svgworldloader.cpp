// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_svgworldloader.cpp by Victor Dods, created 2004/08/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_svgworldloader.hpp"

#include <iomanip>
#include <set>
#include <sstream>

#include "lvd_xml.hpp"
#include "lvd_xml_parser.hpp"
#include "xrb_engine2_animatedsprite.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_engine2_world.hpp"
#include "xrb_filesystem.hpp"
#include "xrb_pal.hpp"
#include "xrb_util.hpp"

using namespace Lvd;
using namespace Xml;

namespace Xrb {
namespace Engine2 {

struct LoadSvgIntoWorldContext
{
public:

    // set during construction
    std::string const &m_svg_path;
    World &m_world;
    Time const m_current_time;
    Uint32 const m_gltexture_flags;
    Uint32 const m_stage;
    std::string const m_additional_stageable_attribute_name_prefix;
    std::string const m_resource_path_prefix;
    // set later on
    Document *m_document;
    Element *m_svg;
    Element *m_g;
    Uint32 m_layer_number;
    ObjectLayer *m_object_layer;
    Uint32 m_stage_count;

    LoadSvgIntoWorldContext (
        std::string const &svg_path,
        World &world,
        Time current_time,
        Uint32 gltexture_flags,
        Uint32 stage,
        std::string const &additional_stageable_attribute_name_prefix,
        std::string const &resource_path_prefix)
        :
        m_svg_path(svg_path),
        m_world(world),
        m_current_time(current_time),
        m_gltexture_flags(gltexture_flags),
        m_stage(stage),
        m_additional_stageable_attribute_name_prefix(additional_stageable_attribute_name_prefix),
        m_resource_path_prefix(resource_path_prefix),
        m_document(NULL),
        m_svg(NULL),
        m_g(NULL),
        m_layer_number(0),
        m_object_layer(NULL),
        m_stage_count(0)
    { }
}; // end of struct LoadSvgIntoWorldContext

void StageProcessAttributes (
    Element &element,
    Uint32 stage,
    Uint32 stage_count,
    std::string const &additional_stageable_attribute_name_prefix)
{
    // if stage processing is disabled (stage == 0), do nothing
    if (stage == 0)
        return;

    // if stage processing is enabled, check that stage is a valid number
    if (stage > stage_count)
        throw Exception("stage value exceeds stage_count value");

    std::set<std::string> stageable_attribute_name_prefixes;
    stageable_attribute_name_prefixes.insert("xrb_");
    if (!additional_stageable_attribute_name_prefix.empty())
        stageable_attribute_name_prefixes.insert(additional_stageable_attribute_name_prefix);

    for (AttributeMap::iterator it = element.m_attribute.begin(),
                                it_end = element.m_attribute.end();
         it != it_end;
         ++it)
    {
        std::string const &attribute_name = it->first;
        std::string &attribute_value = it->second;

        bool accepted = false;
        // if the attribute name has one of the allowable attribute name prefixes, accept
        for (std::set<std::string>::const_iterator prefix_it = stageable_attribute_name_prefixes.begin(),
                                                   prefix_it_end = stageable_attribute_name_prefixes.end();
             prefix_it != prefix_it_end;
             ++prefix_it)
        {
            if (attribute_name.find_first_of(*prefix_it) == 0)
            {
                accepted = true;
                break;
            }
        }

        // if the attribute value is a backslash-delimited list of values (i.e. if the
        // first char is backslash), replace it with the one corresponding to stage.
        if (!attribute_value.empty() && attribute_value[0] == '\\')
        {
            std::string::size_type pos = 0;
            std::string::size_type next = 0;

            // first count the number of backslash-delimited values so we can validate stage_count
            Uint32 actual_stage_count = 0;
            for (Uint32 i = 0; i < attribute_value.size(); ++i)
                if (attribute_value[i] == '\\')
                    ++actual_stage_count;

            if (actual_stage_count != stage_count)
                throw Exception(FORMAT("element at " << element.m_filoc << ", attribute name '" << attribute_name << "': number of backslash-delimited values differs from value of xrb_stage_count"));

            Uint32 s = stage;
            while (s-- > 0)
                pos = attribute_value.find_first_of('\\', pos) + 1;
            next = attribute_value.find_first_of('\\', pos);

            // cut out everything but the desired stage's attribute value
            attribute_value = attribute_value.substr(pos, next-1);
        }
    }
}

void StageProcessAttributes (LoadSvgIntoWorldContext &context, Element &element) throw(Exception)
{
    StageProcessAttributes(element, context.m_stage, context.m_stage_count, context.m_additional_stageable_attribute_name_prefix);
}

// AttributeMap is a conveniently pre-defined typedef for this
void ParseStyle (std::string const &style, AttributeMap &style_map)
{
    // style consists of a semicolon-delimited list of "name:value" entries
    std::string::size_type p = 0;
    while (p < style.size())
    {
        // don't worry too much about malformed input
        std::string::size_type next_p = style.find_first_of(":", p);
        if (next_p == std::string::npos)
            break;

        std::string name(style.substr(p, next_p-p));
        p = next_p + 1;
        next_p = style.find_first_of(";", p);
        std::string value(style.substr(p, next_p-p));

        style_map[name] = value;

        if (next_p == std::string::npos)
            break;

        p = next_p + 1;
    }
}

void ParseColorFromStyle (std::string const &style, Color &fill_color, Color &opacity_mask)
{
    AttributeMap style_map;
    ParseStyle(style, style_map);

    // default fill is 'none', meaning transparent [black]
    fill_color = Color::ms_transparent_black;
    AttributeMap::const_iterator it = style_map.find("fill");
    if (it != style_map.end() && it->second != "none")
    {
        Uint32 fill_value = 0x000000;
        {
            char c;
            std::istringstream in(it->second);
            in >> c; // there is a leading '#'
            in >> std::hex >> fill_value;
        }

        Float fill_opacity = 0.0f;
        it = style_map.find("fill-opacity");
        if (it != style_map.end())
        {
            std::istringstream in(it->second);
            in >> fill_opacity; // floating point value
        }

        Float red   = (fill_value >> 16) & 0xFF;
        Float green = (fill_value >>  8) & 0xFF;
        Float blue  = (fill_value >>  0) & 0xFF;

        fill_color = Color(red/255.0f, green/255.0f, blue/255.0f, fill_opacity);
    }

    // opacity is the object opacity (which applies to sprites also).  default is totally opaque.
    opacity_mask = Color::ms_opaque_white;
    Float opacity = 1.0f;
    it = style_map.find("opacity");
    if (it != style_map.end())
    {
        std::istringstream in(it->second);
        in >> opacity; // floating point value
    }

    opacity_mask[Dim::A] = opacity;
}

std::string const &GetRequiredAttributeOrThrow (
    Element const &element,
    std::string const &attribute_name,
    std::string const &explanation = "")
{
    std::string filoc_string(element.m_filoc.AsString());
    if (!filoc_string.empty())
        filoc_string = " (at " + filoc_string + ")";
    if (!element.HasAttribute(attribute_name))
        throw Exception(FORMAT("<" << element.m_name << ">" << filoc_string << " is missing attribute '" << attribute_name << "'" << (!explanation.empty() ? " (" + explanation + ")" : "")));
    return element.AttributeValue(attribute_name);
}

void ProcessImage (
    LoadSvgIntoWorldContext &context,
    FloatMatrix2 const &change_of_basis,
    FloatMatrix2 const &bounding_box_transform,
    Float bounding_box_half_side_length,
    Uint32 image_index,
    Element &image)
{
    ASSERT1(context.m_document != NULL);
    ASSERT1(context.m_svg != NULL);
    ASSERT1(context.m_g != NULL);
    ASSERT1(context.m_object_layer != NULL);

    // stage-processing could affect the xrb_ignore attribute, so do it first
    StageProcessAttributes(context, image);

    // if we find attribute xrb_ignore='true' then ignore this element
    if (image.AttributeValue("xrb_ignore") == "true")
        return;

    std::string image_id(FORMAT("unnamed image at line " << image.m_filoc.LineNumber()));

    try {

        // id of <image> element, and invisible, image_path and animation_path attributes
        image_id = GetRequiredAttributeOrThrow(image, "id");
    //             bool is_invisible = image.AttributeValue("xrb_invisible") == "true";
        std::string image_path(GetRequiredAttributeOrThrow(image, "xlink:href", "attribute 'xlink:href' must specify the relative path of the sprite image file"));
        std::string animation_path(image.AttributeValue("xrb_animation_path"));

        // retrieve and parse the width, height, x and y values
        Float width = Util::TextToFloat(GetRequiredAttributeOrThrow(image, "width").c_str());
        Float height = Util::TextToFloat(GetRequiredAttributeOrThrow(image, "height").c_str());
        Float pre_translate_x = Util::TextToFloat(GetRequiredAttributeOrThrow(image, "x").c_str());
        Float pre_translate_y = Util::TextToFloat(GetRequiredAttributeOrThrow(image, "y").c_str());
        FloatMatrix2 pre_translate(1.0f, 0.0f, pre_translate_x, 0.0f, 1.0f, pre_translate_y);
        FloatMatrix2 origin_transform(
            0.5f*width, 0.0f,  0.5f*width,
            0.0f, 0.5f*height, 0.5f*height);

        // allow a small variance in the size of the image, but it should be square
        if (Abs(width - height) > 0.01f)
            throw Exception(FORMAT("'width' and 'height' attributes for <image id='" << image_id << "'> are not equal but must be"));
    //             // take the average of them just for good measure
    //             Float square_size = 0.5f * (width + height);

        // parse transform_string -- could be empty, or contain
        // translate(x,y) or matrix(a,b,c,d,x,y)
        std::string transform_string(image.AttributeValue("transform"));
        FloatMatrix2 object_transform(FloatMatrix2::ms_identity);
        if (transform_string.empty())
        {
            // identity transform
        }
        else if (transform_string.find("translate(") == 0)
        {
            // translation only
            std::istringstream in(transform_string);
            for (Uint32 i = 0; i < 10 /* this is strlen("translate(")*/; ++i)
                in.get();
            char c;
            // number ',' number ')'
            in >> object_transform[FloatMatrix2::X] >> c >> object_transform[FloatMatrix2::Y] >> c;
        }
        else if (transform_string.find("matrix(") == 0)
        {
            // check for skew transforms (these are not allowed by XRB)

            // everything -- attempt to fit 6-degree-of-freedom matrix (with
            // possible skew) as 5-degree-of-freedom matrix (no skew)
            std::istringstream in(transform_string);
            for (Uint32 i = 0; i < 7 /* this is strlen("matrix(")*/; ++i)
                in.get();
            char c;
            FloatMatrix2 m;
            // numbers separated by ',', ended with ')'
            in >> object_transform[FloatMatrix2::A] >> c
               >> object_transform[FloatMatrix2::C] >> c
               >> object_transform[FloatMatrix2::B] >> c
               >> object_transform[FloatMatrix2::D] >> c
               >> object_transform[FloatMatrix2::X] >> c
               >> object_transform[FloatMatrix2::Y] >> c;

            FloatTransform2 dummy(true); // post-translate, although FitMatrix2 sets this anyway
            Float angle_variance = dummy.FitMatrix2(object_transform);
            if (angle_variance > 0.01f)
                throw Exception(FORMAT("skew transformation detected (angle variance = " << angle_variance << "); no skew transformation allowed, because then the sprite angle is undefined"));
        }
        else
            throw Exception(FORMAT("malformed 'transform' attribute in <image id='" << image_id << "'>; must be of form 'translate(x,y)' or 'matrix(a,b,c,d,x,y)'"));

        // determine the total transformation
        FloatTransform2 transform(FloatTransform2::ms_identity);
        {
            FloatMatrix2 m(
                change_of_basis * // this is really the inverse of change_of_basis (which equals itself)
                bounding_box_transform *
                object_transform *
                pre_translate *
                origin_transform *
                change_of_basis);
            // attempt to fit the total transformation to a Transform2
            Float angle_variance = transform.FitMatrix2(m);
            if (angle_variance > 0.01f)
            {
                throw Exception(FORMAT("skew transformation detected (angle variance = " << angle_variance << "); no skew transformation allowed, because then the sprite angle is undefined"));
            }
        }

        // check that the center of the would-be sprite is inside the
        // bounding box for the layer.
        if (transform.Translation()[Dim::X] < -bounding_box_half_side_length ||
            transform.Translation()[Dim::X] >  bounding_box_half_side_length ||
            transform.Translation()[Dim::Y] < -bounding_box_half_side_length ||
            transform.Translation()[Dim::Y] >  bounding_box_half_side_length)
        {
            throw Exception("<image> center lies outside the bounding box for this layer");
        }

        // create the Object
        Object *object = NULL;
        if (!animation_path.empty())
        {
            std::string prefixed_animation_path(context.m_resource_path_prefix);
            prefixed_animation_path += animation_path;
            object = AnimatedSprite::Create(prefixed_animation_path, context.m_current_time, context.m_gltexture_flags);
            if (object == NULL)
                std::cerr << "LoadSvgIntoWorld(\"" << context.m_svg_path << "\"); failed to load animation \"" << prefixed_animation_path << '"' << std::endl;
        }
        // if nothing was loaded so far, attempt to load the image as a sprite
        {
            std::string prefixed_image_path(context.m_resource_path_prefix);
            prefixed_image_path += image_path;
            if (object == NULL)
                object = Sprite::Create(prefixed_image_path, context.m_gltexture_flags);
            if (object == NULL)
                throw Exception(FORMAT("failed to load image \"" << prefixed_image_path << "\""));
            ASSERT1(object->GetEntity() == NULL && "no Entity allowed at this point");
        }

        // set the transform
        static_cast<FloatTransform2 &>(*object) = transform;
        // set the z-depth
        object->SetZDepth(-Float(image_index)); // this needs to be used if USE_SOFTWARE_TRANSFORM is 1 (in xrb_engine2_sprite.cpp)
//         object->SetZDepth(1.0f - (1.0f / 65536.0f) * image_index); // this needs to be used if USE_SOFTWARE_TRANSFORM is 0 (in xrb_engine2_sprite.cpp)
        // set the color mask from the style attribute
        if (image.HasAttribute("style"))
        {
            Color fill_color;
            Color opacity_mask;
            ParseColorFromStyle(image.AttributeValue("style"), fill_color, opacity_mask);
            object->ColorBias() = fill_color;
            object->ColorMask() = opacity_mask;
        }
        // set the is-transparent flag
        object->SetIsTransparent(image.AttributeValue("xrb_is_transparent") == "true");

        // if xrb_entity_type is present, attempt to create and attach an Entity
        if (image.HasAttribute("xrb_entity_type"))
        {
            std::string entity_type(image.AttributeValue("xrb_entity_type"));
            // optional attribute which specifies a handle with which to retrieve the Entity pointer
            std::string entity_name(image.AttributeValue("xrb_entity_name"));

            Entity *entity = NULL;
            ASSERT1(object != NULL);

            try {
                entity = context.m_world.CreateEntity(entity_type, entity_name, *object, *context.m_object_layer, image);
                if (entity != NULL)
                {
                    // set the Entity and add it to the world
                    object->SetEntity(entity);
                    context.m_world.AddDynamicObject(object, context.m_object_layer);
                }
                else
                {
                    // if the returned entity was NULL, it signifies that there should be no entity
                    // associated with this image, and a static sprite should be used instead.
                    context.m_world.AddStaticObject(object, context.m_object_layer);
                }
            } catch (Exception const &e) {
                // if an exception was thrown, add it as a static object (no Entity attached)
                context.m_world.AddStaticObject(object, context.m_object_layer);
                // then rethrow the exception because we still want the exception printed.
                throw e;
            }
        }
        // if it wasn't an Entity, then add it to the World as a static object.
        else
        {
            context.m_world.AddStaticObject(object, context.m_object_layer);
        }

    } catch (Exception const &e) {
        std::cerr << "LoadSvgIntoWorld(\"" << context.m_svg_path << "\"); in <image id='" << image_id << "'>, layer '" << context.m_object_layer->Name() << "' (line " << image.m_filoc.LineNumber() << " in svg file): " << e.what() << std::endl;
    }
}

void ProcessLayer (LoadSvgIntoWorldContext &context)
{
    ASSERT1(context.m_document != NULL);
    ASSERT1(context.m_svg != NULL);
    ASSERT1(context.m_g != NULL);

    // stage-processing could affect the xrb_ignore attribute, so do it first
    StageProcessAttributes(context, *context.m_g);

    // if we find attribute ignore='true' then ignore this element
    if (context.m_g->AttributeValue("xrb_ignore") == "true")
        return;

    std::string layer_id(FORMAT("unnamed layer (#" << context.m_layer_number << ")"));

    try {

        // layer id
        layer_id = GetRequiredAttributeOrThrow(*context.m_g, "id", FORMAT("layer " << context.m_layer_number << " has no 'id' attribute"));

        // xrb_bounding_box
        std::string bounding_box_id(GetRequiredAttributeOrThrow(*context.m_g, "xrb_bounding_box", "missing attribute xrb_bounding_box; must specify the id of a <rect> element in this layer"));
        Element const *bounding_box = NULL;
        context.m_g->FirstElement(bounding_box, "rect", "id", true, bounding_box_id);
        if (bounding_box == NULL)
            throw Exception(FORMAT("layer '" << layer_id << "' has invalid xrb_bounding_box='" << bounding_box_id << "'; must specify the id of a <rect> element in this layer"));
        if (bounding_box->HasAttribute("transform"))
            throw Exception(FORMAT("layer '" << layer_id << "' has invalid xrb_bounding_box='" << bounding_box_id << "'; cannot have a 'transform' attribute"));
        Float bounding_box_width = Util::TextToFloat(GetRequiredAttributeOrThrow(*bounding_box, "width").c_str());
        Float bounding_box_height = Util::TextToFloat(GetRequiredAttributeOrThrow(*bounding_box, "height").c_str());

        // ensure bounding box is square
        if (Abs(bounding_box_width - bounding_box_height) > 0.01f)
            throw Exception(FORMAT("'width' and 'height' attributes for bounding box <rect id='" << bounding_box_id << "'> are not equal but must be"));
        // take the average of them just for good measure
        Float bounding_box_side_length = 0.5f * (bounding_box_width + bounding_box_height);
        Float bounding_box_half_side_length = 0.5f * bounding_box_side_length;

        // NOTE: the coordinates in the SVG file are left-handed, and XRB uses right-handed.
        // we must conjugate by a matrix representing the change of basis (left to righthanded).
        // also include the bounding box transformation in the outer, so that our origin is
        // the center of the bounding box.  Note that the inverse of the following matrix is
        // itself.
        FloatMatrix2 change_of_basis(
            1.0f,  0.0f, 0.0f,
            0.0f, -1.0f, 0.0f);

        // read x/y offset for bounding box (this needs to be used to transform
        // the x/y post-translate coordinates of subordinate images) -- the coordinates
        // indicate the location of the upper-left corner of the box.
        Float bounding_box_x = Util::TextToFloat(GetRequiredAttributeOrThrow(*bounding_box, "x").c_str());
        Float bounding_box_y = Util::TextToFloat(GetRequiredAttributeOrThrow(*bounding_box, "y").c_str());
        FloatVector2 bounding_box_center(bounding_box_x + bounding_box_half_side_length, bounding_box_y + bounding_box_half_side_length);
        FloatMatrix2 bounding_box_transform(
            1.0f,  0.0f, -bounding_box_center[Dim::X],
            0.0f,  1.0f, -bounding_box_center[Dim::Y]);

        // xrb_z_depth
        Float z_depth = 0.0f;
        if (context.m_g->HasAttribute("xrb_z_depth"))
        {
            std::string z_depth_string(context.m_g->AttributeValue("xrb_z_depth"));
            z_depth = Util::TextToFloat(z_depth_string.c_str());
        }

        // xrb_quadtree_depth
        Uint32 quadtree_depth = 5; // arbitrary default
        if (context.m_g->HasAttribute("xrb_quadtree_depth"))
        {
            std::string quadtree_depth_string(context.m_g->AttributeValue("xrb_quadtree_depth"));
            quadtree_depth = Util::TextToUint<Uint32>(quadtree_depth_string.c_str());
        }

        // create ObjectLayer with info from above, and add it to the World
        ObjectLayer *object_layer = context.m_world.CreateObjectLayer(bounding_box_side_length, quadtree_depth, z_depth, context.m_g->AttributeValue("id"), *context.m_g);
        context.m_world.AddObjectLayer(object_layer);
        context.m_object_layer = object_layer;

        // parse the style so we can get the background color for this ObjectLayer
        if (bounding_box->HasAttribute("style"))
        {
            Color fill_color;
            Color opacity_mask;
            ParseColorFromStyle(bounding_box->AttributeValue("style"), fill_color, opacity_mask);
            object_layer->SetBackgroundColor(fill_color * opacity_mask);
        }

        // read the contents
        Element *image = NULL;
        Uint32 image_index = 0;
        for (DomNodeVector::iterator image_it = context.m_g->FirstElement(image, "image");
             image_it != context.m_g->m_element.end();
             context.m_g->NextElement(image_it, image, "image"), ++image_index)
        {
            ASSERT1(image != NULL);
            ProcessImage(context, change_of_basis, bounding_box_transform, bounding_box_half_side_length, image_index, *image);
        }

        context.m_object_layer = NULL; // done with this layer

    } catch (Exception const &e) {
        std::cerr << "LoadSvgIntoWorld(\"" << context.m_svg_path << "\"); in layer '" << layer_id << "': " << e.what() << std::endl;
    }
}

void LoadSvgIntoWorld (LoadSvgIntoWorldContext &context) throw(Exception)
{
    DomNode *root = NULL;

    Parser parser;
    bool open_file_success = parser.OpenFile(Singleton::FileSystem().OsPath(context.m_svg_path, FileSystem::READ_ONLY));
    if (!open_file_success)
        throw Exception("failure opening file");

    parser.WarningAndErrorLogStream(&std::cerr);
    Time start_time = Singleton::Pal().CurrentTime();
    Parser::ParserReturnCode parser_return_code = parser.Parse(&root);
    Time end_time = Singleton::Pal().CurrentTime();
    std::cerr << "LoadSvgIntoWorld(\"" << context.m_svg_path << "\"); parse time = " << end_time - start_time << " s" << std::endl;
    if (parser_return_code != Parser::PRC_SUCCESS)
        throw Exception("general parse error in file");

    start_time = Singleton::Pal().CurrentTime();
    ASSERT1(root != NULL);
    ASSERT1(root->m_type == DomNode::DOCUMENT);
    context.m_document = DStaticCast<Document *>(root);

    // extract the single <svg> element which contains everything
    Element *svg = NULL;
    context.m_document->FirstElement(svg, "svg");
    if (svg == NULL) // if no <svg> element, error.
        throw Exception("no <svg> tag -- probably not an svg document");
    context.m_svg = svg;

    // validation of stage processing, if enabled
    if (context.m_stage > 0)
    {
        if (!context.m_svg->HasAttribute("xrb_stage_count"))
            throw Exception("stage processing is enabled, but the 'xrb_stage_count' attribute is missing from the <svg> element");

        std::string const &stage_count_string = context.m_svg->AttributeValue("xrb_stage_count");
        if (!stage_count_string.empty() && stage_count_string[0] == '\\')
            throw Exception("the 'xrb_stage_count' attribute must be numeric; in particular, it may not begin with a backslash");

        context.m_stage_count = Util::TextToUint<Uint32>(context.m_svg->AttributeValue("xrb_stage_count").c_str());

        if (context.m_stage > context.m_stage_count)
            throw Exception("requested stage exceeds value of 'xrb_stage_count' attribute");
    }

    // exceptions thrown in this block produce non-critical errors, hence the try/catch
    try {
        StageProcessAttributes(context, *context.m_svg);
    } catch (Exception const &e) {
        std::cerr << "LoadSvgIntoWorld(\"" << context.m_svg_path << "\"); in <svg> element: " << e.what() << std::endl;
    }

    // give the world a chance to process the svg element
    context.m_world.ProcessSvgRootElement(*svg);

    // iterate through each <g> element for the object layers
    Element *g = NULL;
    context.m_layer_number = 0;
    for (DomNodeVector::iterator g_it = svg->FirstElement(g, "g");
         g_it != svg->m_element.end();
         svg->NextElement(g_it, g, "g"), ++context.m_layer_number)
    {
        ASSERT1(g != NULL);
        context.m_g = g;
        ProcessLayer(context);
        context.m_g = NULL; // done with this g
    }
    end_time = Singleton::Pal().CurrentTime();
    std::cerr << "LoadSvgIntoWorld(\"" << context.m_svg_path << "\"); process time = " << end_time - start_time << " s" << std::endl;

    Delete(root);
}

Uint32 ParseSvgStageCount (std::string const &svg_path) throw(Exception)
{
    Parser parser;
//     parser.DebugSpew(true);
    bool open_file_success = parser.OpenFile(svg_path);
    if (!open_file_success)
        throw Exception("failure opening file");

    parser.WarningAndErrorLogStream(&std::cerr);
    Element *root = parser.ParseOnlyRootElement();
    if (root == NULL)
        throw Exception("general parse error in file");

    if (root->m_name != "svg")
        throw Exception("no <svg tag -- probably not an svg document");

    Uint32 stage_count = 0;
    if (root->HasAttribute("xrb_stage_count"))
        stage_count = Util::TextToUint<Uint32>(root->AttributeValue("xrb_stage_count").c_str());
    Delete(root);
    return stage_count;
}

void LoadSvgIntoWorld (
    std::string const &svg_path,
    World &world,
    Time current_time,
    Uint32 gltexture_flags,
    Uint32 stage,
    std::string const &additional_stageable_attribute_name_prefix,
    std::string const &resource_path_prefix) throw(Exception)
{
    LoadSvgIntoWorldContext context(svg_path, world, current_time, gltexture_flags, stage, additional_stageable_attribute_name_prefix, resource_path_prefix);
    LoadSvgIntoWorld(context);
}

} // end of namespace Engine2
} // end of namespace Xrb
