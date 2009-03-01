// ///////////////////////////////////////////////////////////////////////////
// xrb_huffman.cpp by Victor Dods, created 2005/05/09
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_huffman.hpp"

#include <stdlib.h>

#include "xrb_serializer.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// HuffmanTreeNode
// ///////////////////////////////////////////////////////////////////////////

class HuffmanTreeNode
{
public:

    enum
    {
        AVAILABLE_BRANCH_NODE_WEIGHT = 0xFFFFFFFF,
        PARENTED_NODE_WEIGHT         = AVAILABLE_BRANCH_NODE_WEIGHT - 1,
        BRANCH_NODE_DATA_VALUE       = 0xFFFFFFFF
    };

    HuffmanTreeNode *m_child[2];

    Uint32 m_weight;
    Uint32 m_data;

    HuffmanTreeNode ();
    ~HuffmanTreeNode ();

    bool GetIsLeafNode () const { return m_data != BRANCH_NODE_DATA_VALUE; }

    static int QsortCompare (void const *a, void const *b);

    void TraverseAndBuildCodes (Huffman::Code *const codes) const;
    void Fprint (FILE *fptr, Uint32 const indent_level = 0) const;

private:

    void TraverseAndBuildCodesPrivate (
        Huffman::Code *const codes,
        Huffman::Code *const current_code) const;
}; // end of class HuffmanTreeNode

HuffmanTreeNode::HuffmanTreeNode ()
{
    m_child[0] = NULL;
    m_child[1] = NULL;

    m_weight = 0;
    m_data = 0;
}

HuffmanTreeNode::~HuffmanTreeNode ()
{
    Delete(m_child[0]);
    Delete(m_child[1]);
}

int HuffmanTreeNode::QsortCompare (void const *a, void const *b)
{
    HuffmanTreeNode const *node_a = *static_cast<HuffmanTreeNode const *const *>(a);
    HuffmanTreeNode const *node_b = *static_cast<HuffmanTreeNode const *const *>(b);

    ASSERT1(node_a != NULL);
    ASSERT1(node_b != NULL);

    if (node_a->m_weight < node_b->m_weight)
        return -1;
    else if (node_a->m_weight == node_b->m_weight)
        return 0;
    else
        return 1;
}

void HuffmanTreeNode::TraverseAndBuildCodes (
    Huffman::Code *const codes) const
{
    ASSERT1(codes != NULL);

    Huffman::Code current_code;
    TraverseAndBuildCodesPrivate(codes, &current_code);
}

void HuffmanTreeNode::TraverseAndBuildCodesPrivate (
    Huffman::Code *const codes,
    Huffman::Code *const current_code) const
{
    ASSERT2(codes != NULL);
    ASSERT1(current_code != NULL);

    if (GetIsLeafNode())
    {
        ASSERT1(m_data < 256);
        codes[m_data] = *current_code;
        codes[m_data].m_sequence >>= 256 - current_code->m_length;
    }
    else
    {
        // it should never get deeper than 255 without hitting a leaf
        ASSERT1(current_code->m_length < 255);

        current_code->m_sequence.SetBit(255 - current_code->m_length, 1);
        ++current_code->m_length;
        m_child[1]->TraverseAndBuildCodesPrivate(codes, current_code);
        --current_code->m_length;

        // doing the 0 child second ensures that the rest of
        // the current_sequence will remain all zeros.
        current_code->m_sequence.SetBit(255 - current_code->m_length, 0);
        ++current_code->m_length;
        m_child[0]->TraverseAndBuildCodesPrivate(codes, current_code);
        --current_code->m_length;

        // ensure the trailing bits in the current code sequence
        // are actually all zero
        ASSERT1((current_code->m_sequence << current_code->m_length)
                ==
                BitArray256::ms_zero);
    }
}

void HuffmanTreeNode::Fprint (FILE *fptr, Uint32 const indent_level) const
{
    if (m_child[0] != NULL)
    {
        m_child[0]->Fprint(fptr, indent_level + 1);
        ASSERT1(m_child[1] != NULL);
    }

    for (Uint32 i = 0; i < indent_level; ++i)
        fprintf(stderr, "  ");
    fprintf(fptr,
            "m_data = %c\n",
            (m_data >= 32 && m_data < 128) ? static_cast<char>(m_data) : '.');

    if (m_child[1] != NULL)
    {
        m_child[1]->Fprint(fptr, indent_level + 1);
        ASSERT1(m_child[0] != NULL);
    }
}

// ///////////////////////////////////////////////////////////////////////////
// Huffman
// ///////////////////////////////////////////////////////////////////////////

Huffman::Huffman (Uint32 const *byte_weights)
{
    ConstructTree(byte_weights);
    m_tree->TraverseAndBuildCodes(m_codes);
}

Huffman::~Huffman ()
{
    Delete(m_tree);
}

Uint32 Huffman::EncodeBytes (
    Uint8 const *source,
    Uint32 source_size,
    Serializer &serializer) const
{
    ASSERT1(source != NULL);
    ASSERT1(source_size > 0);
    ASSERT1(serializer.GetIsOpen());
    ASSERT1(serializer.GetIODirection() == IOD_WRITE);

    for (Uint32 i = 0; i < source_size; ++i)
    {
        serializer.WriteBitArray<256>(
            m_codes[*source].m_sequence,
            m_codes[*source].m_length);
        if (serializer.GetError() != IOE_NONE)
            return source_size - i;
        ++source;
    }
    return source_size;
}

Uint32 Huffman::DecodeBytes (
    Uint8 *destination,
    Uint32 destination_size,
    Serializer &serializer) const
{
    ASSERT1(destination != NULL);
    ASSERT1(destination_size > 0);
    ASSERT1(serializer.GetIsOpen());
    ASSERT1(serializer.GetIODirection() == IOD_READ);

    Uint32 bytes_read;
    HuffmanTreeNode const *node;
    bool bit;
    for (bytes_read = 0; bytes_read < destination_size; ++bytes_read)
    {
        node = m_tree;
        ASSERT1(node != NULL);
        while (!node->GetIsLeafNode())
        {
            bit = serializer.ReadBool();
            if (serializer.GetError() != IOE_NONE)
                break;
            node = node->m_child[bit ? 1 : 0];
            ASSERT1(node != NULL);
        }
        if (serializer.GetError() != IOE_NONE)
            break;
        ASSERT1(node->m_data < 256);
        *destination = node->m_data;
        ++destination;
    }

    return bytes_read;
}

void Huffman::FprintCompressedByteWeights (FILE *fptr) const
{
    fprintf(fptr, "Huffman @0x%p - compressed byte weights\n", this);
    for (Uint32 i = 0; i < 256; ++i)
        fprintf(fptr,
                "'%c' (%3d / %02X) bits = %u\n",
                (i >= ' ' && i <= '~') ? i : '.',
                i,
                i,
                m_codes[i].m_length);
}

void Huffman::ConstructTree (Uint32 const *byte_weights)
{
    HuffmanTreeNode *nodes[TOTAL_NODE_COUNT];

    for (Uint32 i = 0; i < LEAF_NODE_COUNT; ++i)
    {
        nodes[i] = new HuffmanTreeNode;

        ASSERT1(byte_weights[i] < HuffmanTreeNode::PARENTED_NODE_WEIGHT);
        nodes[i]->m_weight = byte_weights[i];
        nodes[i]->m_data = i;
    }
    for (Uint32 i = LEAF_NODE_COUNT; i < TOTAL_NODE_COUNT; ++i)
    {
        nodes[i] = new HuffmanTreeNode;

        nodes[i]->m_weight = HuffmanTreeNode::AVAILABLE_BRANCH_NODE_WEIGHT;
        nodes[i]->m_data = HuffmanTreeNode::BRANCH_NODE_DATA_VALUE;
    }

    Uint32 const branch_node_index = TOTAL_NODE_COUNT - 1;
    Uint32 nodes_left = LEAF_NODE_COUNT;
    while (nodes_left > 1)
    {
        qsort(nodes,
              TOTAL_NODE_COUNT,
              sizeof(HuffmanTreeNode *),
              HuffmanTreeNode::QsortCompare);

        // make sure the supposed branch node actually is one
        ASSERT1(nodes[branch_node_index]->m_weight
                ==
                HuffmanTreeNode::AVAILABLE_BRANCH_NODE_WEIGHT);
        // add the lowest-weight nodes to an available branch node
        // and sum their weights to make the combined weight for
        // the branch node.
        nodes[branch_node_index]->m_child[0] = nodes[0];
        nodes[branch_node_index]->m_child[1] = nodes[1];
        // make sure the sum of weights isn't higher than the
        // sentinel weight values.
        ASSERT1(nodes[branch_node_index]->m_child[0]->m_weight +
                nodes[branch_node_index]->m_child[1]->m_weight
                <
                HuffmanTreeNode::PARENTED_NODE_WEIGHT);
        // also make sure the sum of weights didn't wrap around.
        ASSERT1(nodes[branch_node_index]->m_child[0]->m_weight +
                nodes[branch_node_index]->m_child[1]->m_weight
                >=
                nodes[branch_node_index]->m_child[0]->m_weight);
        // sum the weights of the children to form the weight of the parent
        nodes[branch_node_index]->m_weight =
            nodes[branch_node_index]->m_child[0]->m_weight +
            nodes[branch_node_index]->m_child[1]->m_weight;
        // change their weights such that they are no longer
        // sorted in with the other nodes
        nodes[branch_node_index]->m_child[0]->m_weight =
            HuffmanTreeNode::PARENTED_NODE_WEIGHT;
        nodes[branch_node_index]->m_child[1]->m_weight =
            HuffmanTreeNode::PARENTED_NODE_WEIGHT;

        --nodes_left;
    }

    m_tree = nodes[branch_node_index];
    ASSERT1(branch_node_index == TOTAL_NODE_COUNT - 1);
    ASSERT1(m_tree->m_weight != HuffmanTreeNode::AVAILABLE_BRANCH_NODE_WEIGHT);
}

} // end of namespace Xrb
