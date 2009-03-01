// ///////////////////////////////////////////////////////////////////////////
// xrb_huffman.hpp by Victor Dods, created 2005/05/09
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_HUFFMAN_HPP_)
#define _XRB_HUFFMAN_HPP_

#include "xrb.hpp"

#include <stdio.h>

#include "xrb_bitarray.hpp"

namespace Xrb
{

// temp for now, until a formal BitArray typedef header/implementation is made
typedef BitArray<256> BitArray256;

class HuffmanTreeNode;
class Serializer;

// implements Huffman en/decoding for (de)compression of bytes.
class Huffman
{
public:

    Huffman (Uint32 const *byte_weights);
    ~Huffman ();

    Uint32 EncodeBytes (
        Uint8 const *source,
        Uint32 source_size,
        Serializer &serializer) const;
    Uint32 DecodeBytes (
        Uint8 *destination,
        Uint32 destination_size,
        Serializer &serializer) const;

    void FprintCompressedByteWeights (FILE *fptr) const;

private:

    // a specific bit sequence (and length) for each byte
    struct Code
    {
        // bit sequence for the applicable byte
        BitArray256 m_sequence;
        // the number of (least significant) bits to use from m_sequence
        Uint8 m_length;

        inline Code ()
            :
            m_sequence(BitArray256::ms_zero),
            m_length(0)
        { }
        inline ~Code () { }
    }; // end of struct HuffmanCode

    // 256 codes, one for each byte
    Code m_codes[256];

    enum
    {
        LEAF_NODE_COUNT = 256,
        BRANCH_NODE_COUNT = 255,
        TOTAL_NODE_COUNT = LEAF_NODE_COUNT + BRANCH_NODE_COUNT
    };

    HuffmanTreeNode const *m_tree;

    void ConstructTree (Uint32 const *byte_weights);

    friend class HuffmanTreeNode;
}; // end of class Huffman

} // end of namespace Xrb

#endif // !defined(_XRB_HUFFMAN_HPP_)
