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
#include <sstream>

#include "lvd_xml.hpp"
#include "lvd_xml_parser.hpp"
#include "xrb_engine2_animatedsprite.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_engine2_world.hpp"
#include "xrb_pal.hpp"
#include "xrb_util.hpp"

using namespace Lvd;
using namespace Xml;

namespace Xrb {
namespace Engine2 {

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

std::string const &GetRequiredAttributeOrThrow (
    Element const &element,
    std::string const &attribute_name,
    std::string const &explanation = "")
{
    std::string filoc_string(element.m_filoc.AsString());
    if (!filoc_string.empty())
        filoc_string = " (at " + filoc_string + ")";
    if (!element.HasAttribute(attribute_name))
        THROW_STRING("<" << element.m_name << ">" << filoc_string << " is missing attribute '" << attribute_name << "'" << (!explanation.empty() ? " (" + explanation + ")" : ""));
    return element.AttributeValue(attribute_name);
}

void ProcessImage (std::string const &svg_path,
                   World &world,
                   Float current_time,
                   ObjectLayer *object_layer,
                   FloatMatrix2 const &change_of_basis,
                   FloatMatrix2 const &bounding_box_transform,
                   Float bounding_box_half_side_length,
                   Uint32 image_index,
                   Element const &image)
{
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
            THROW_STRING("'width' and 'height' attributes for <image id='" << image_id << "'> are not equal but must be");
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
            {
                THROW_STRING("skew transformation detected (angle variance = " << angle_variance << "); no skew transformation allowed, because then the sprite angle is undefined");
            }
        }
        else
            THROW_STRING("malformed 'transform' attribute in <image id='" << image_id << "'>; must be of form 'translate(x,y)' or 'matrix(a,b,c,d,x,y)'");

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
                THROW_STRING("skew transformation detected (angle variance = " << angle_variance << "); no skew transformation allowed, because then the sprite angle is undefined");
            }
        }

        // check that the center of the would-be sprite is inside the
        // bounding box for the layer.
        if (transform.Translation()[Dim::X] < -bounding_box_half_side_length ||
            transform.Translation()[Dim::X] >  bounding_box_half_side_length ||
            transform.Translation()[Dim::Y] < -bounding_box_half_side_length ||
            transform.Translation()[Dim::Y] >  bounding_box_half_side_length)
        {
            THROW_STRING("<image> center lies outside the bounding box for this layer");
        }

        // create the Object
        Object *object = NULL;
        if (!animation_path.empty())
        {
            object = AnimatedSprite::Create(animation_path, current_time);
            if (object == NULL)
                fprintf(stderr, "LoadSvgIntoWorld(\"%s\"); failed to load animation \"%s\"\n", svg_path.c_str(), animation_path.c_str());
        }
        // if nothing was loaded so far, attempt to load the image as a sprite
        if (object == NULL)
            object = Sprite::Create(image_path);
        if (object == NULL)
            THROW_STRING("failed to load image \"" << image_path << "\"");
        ASSERT1(object->GetEntity() == NULL && "no Entity allowed at this point");

        // set the transform
        static_cast<FloatTransform2 &>(*object) = transform;
        // set the z-depth
        object->SetZDepth(-Float(image_index)); // this needs to be used if USE_SOFTWARE_TRANSFORM is 1 (in xrb_engine2_sprite.cpp)
//         object->SetZDepth(1.0f - (1.0f / 65536.0f) * image_index); // this needs to be used if USE_SOFTWARE_TRANSFORM is 0 (in xrb_engine2_sprite.cpp)

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
                entity = world.CreateEntity(entity_type, entity_name, *object, *object_layer, image);
                if (entity == NULL)
                    THROW_STRING("failed to load entity type \"" << entity_type << "\" (entity name \"" << entity_name << "\")");

                // set the Entity and add it to the world
                object->SetEntity(entity);
                world.AddDynamicObject(object, object_layer);
            } catch (std::string const &exception) {
                // if an exception was thrown, add it as a static object (no Entity attached)
                world.AddStaticObject(object, object_layer);
                // then rethrow the exception because we still want the exception printed.
                throw exception;
            }
        }
        // if it wasn't an Entity, then add it to the World as a static object.
        else
        {
            world.AddStaticObject(object, object_layer);
        }

    } catch (std::string const &exception) {
        fprintf(stderr, "LoadSvgIntoWorld(\"%s\"); in <image id='%s'>, layer '%s' (line %u in svg file): %s\n", svg_path.c_str(), image_id.c_str(), object_layer->Name().c_str(), image.m_filoc.LineNumber(), exception.c_str());
    }
}

void ProcessLayer (std::string const &svg_path, World &world, Float current_time, Uint32 layer_number, Element const &g)
{
    // if we find attribute ignore='true' then ignore this element
    if (g.AttributeValue("xrb_ignore") == "true")
        return;

    std::string layer_id(FORMAT("unnamed layer (#" << layer_number << ")"));

    try {

        // layer id
        layer_id = GetRequiredAttributeOrThrow(g, "id", FORMAT("layer " << layer_number << " has no 'id' attribute"));

        // xrb_bounding_box
        std::string bounding_box_id(GetRequiredAttributeOrThrow(g, "xrb_bounding_box", "missing attribute xrb_bounding_box; must specify the id of a <rect> element in this layer"));
        Element const *bounding_box = NULL;
        g.FirstElement(bounding_box, "rect", "id", true, bounding_box_id);
        if (bounding_box == NULL)
            THROW_STRING("layer '" << layer_id << "' has invalid xrb_bounding_box='" << bounding_box_id << "'; must specify the id of a <rect> element in this layer");
        if (bounding_box->HasAttribute("transform"))
            THROW_STRING("layer '" << layer_id << "' has invalid xrb_bounding_box='" << bounding_box_id << "'; cannot have a 'transform' attribute");
        Float bounding_box_width = Util::TextToFloat(GetRequiredAttributeOrThrow(*bounding_box, "width").c_str());
        Float bounding_box_height = Util::TextToFloat(GetRequiredAttributeOrThrow(*bounding_box, "height").c_str());

        // ensure bounding box is square
        if (Abs(bounding_box_width - bounding_box_height) > 0.01f)
            THROW_STRING("'width' and 'height' attributes for bounding box <rect id='" << bounding_box_id << "'> are not equal but must be");
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
        if (g.HasAttribute("xrb_z_depth"))
        {
            std::string z_depth_string(g.AttributeValue("xrb_z_depth"));
            z_depth = Util::TextToFloat(z_depth_string.c_str());
        }

        // xrb_quadtree_depth
        Uint32 quadtree_depth = 5; // arbitrary default
        if (g.HasAttribute("xrb_quadtree_depth"))
        {
            std::string quadtree_depth_string(g.AttributeValue("xrb_quadtree_depth"));
            quadtree_depth = Util::TextToUint<Uint32>(quadtree_depth_string.c_str());
        }

        // create ObjectLayer with info from above, and add it to the World
        ObjectLayer *object_layer = world.CreateObjectLayer(bounding_box_side_length, quadtree_depth, z_depth, g.AttributeValue("id"), g);
        world.AddObjectLayer(object_layer);

        // parse the style so we can get the background color for this ObjectLayer
        if (bounding_box->HasAttribute("style"))
        {
            AttributeMap style_map;
            ParseStyle(bounding_box->AttributeValue("style"), style_map);

            // parse the values
            char c;
            Uint32 fill_value = 0;
            {
                std::istringstream in(style_map["fill"]);
                in >> c; // there is a leading '#'
                in >> std::hex >> fill_value;
            }

            Float fill_opacity = 0.0f;
            {
                std::istringstream in(style_map["fill-opacity"]);
                in >> fill_opacity; // floating point value
            }

            Float red   = (fill_value >> 16) & 0xFF;
            Float green = (fill_value >>  8) & 0xFF;
            Float blue  = (fill_value >>  0) & 0xFF;

            object_layer->SetBackgroundColor(Color(red/255.0f, green/255.0f, blue/255.0f, fill_opacity));
        }

        // read the contents
        Element const *image = NULL;
        Uint32 image_index = 0;
        for (DomNodeVector::const_iterator image_it = g.FirstElement(image, "image");
            image_it != g.m_element.end();
            g.NextElement(image_it, image, "image"), ++image_index)
        {
            ASSERT1(image != NULL);
            ProcessImage(svg_path, world, current_time, object_layer, change_of_basis, bounding_box_transform, bounding_box_half_side_length, image_index, *image);
        }

    } catch (std::string const &exception) {
        fprintf(stderr, "LoadSvgIntoWorld(\"%s\"); in layer '%s': %s\n", svg_path.c_str(), layer_id.c_str(), exception.c_str());
    }
}

void LoadSvgIntoWorld (std::string const &svg_path, World &world, Float current_time)
{
    DomNode *root = NULL;

    try {

        Parser parser;
        bool open_file_success = parser.OpenFile(svg_path);
        if (!open_file_success)
            THROW_STRING("failure opening file");

        parser.WarningAndErrorLogStream(&std::cerr);
        Uint32 start_time = Singleton::Pal().CurrentTime();
        Parser::ParserReturnCode parser_return_code = parser.Parse(&root);
        Uint32 end_time = Singleton::Pal().CurrentTime();
        fprintf(stderr, "LoadSvgIntoWorld(\"%s\"); parse time = %u ms\n", svg_path.c_str(), end_time - start_time);
        if (parser_return_code != Parser::PRC_SUCCESS)
            THROW_STRING("general parse error in file");

        start_time = Singleton::Pal().CurrentTime();
        ASSERT1(root != NULL);
        ASSERT1(root->m_type == DomNode::DOCUMENT);
        Document const &document = *DStaticCast<Document const *>(root);

        // extract the single <svg> element which contains everything
        Element const *svg = NULL;
        document.FirstElement(svg, "svg");
        if (svg == NULL) // if no <svg> element, error.
            THROW_STRING("no <svg> tag -- probably not an svg document");
        // give the world a chance to process it
        world.ProcessSvgRootElement(*svg);

        // iterate through each <g> element for the object layers
        Element const *g = NULL;
        Uint32 layer_number = 0;
        for (DomNodeVector::const_iterator g_it = svg->FirstElement(g, "g");
             g_it != svg->m_element.end();
             svg->NextElement(g_it, g, "g"), ++layer_number)
        {
            ASSERT1(g != NULL);
            ProcessLayer(svg_path, world, current_time, layer_number, *g);
        }
        end_time = Singleton::Pal().CurrentTime();
        fprintf(stderr, "LoadSvgIntoWorld(\"%s\"); process time = %u ms\n", svg_path.c_str(), end_time - start_time);

    } catch (std::string const &exception) {
        fprintf(stderr, "LoadSvgIntoWorld(\"%s\"); %s\n", svg_path.c_str(), exception.c_str());
    }

    delete root;
}

} // end of namespace Engine2
} // end of namespace Xrb
