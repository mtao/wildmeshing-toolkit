
#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include "Tuple.hpp"

namespace wmtk {

//         v2
//       /    \ .
//  e1  /      \  e0
//     v0 - - - v1
//         e2


inline bool Tuple::operator!=(const wmtk::Tuple& t) const
{
    return !(*this == t);
}
inline bool Tuple::operator==(const wmtk::Tuple& t) const
{
#if defined(WMTK_ENABLE_HASH_UPDATE)
    return std::tie(m_local_vid, m_local_eid, m_local_fid, m_global_cid, m_hash) ==
           std::tie(t.m_local_vid, t.m_local_eid, t.m_local_fid, t.m_global_cid, t.m_hash);
#else
    return std::tie(m_local_vid, m_local_eid, m_local_fid, m_global_cid) ==
           std::tie(t.m_local_vid, t.m_local_eid, t.m_local_fid, t.m_global_cid);
#endif

}

inline bool Tuple::operator<(const wmtk::Tuple& t) const
{
#if defined(WMTK_ENABLE_HASH_UPDATE)
    return std::tie(m_local_vid, m_local_eid, m_local_fid, m_global_cid, m_hash) <
           std::tie(t.m_local_vid, t.m_local_eid, t.m_local_fid, t.m_global_cid, t.m_hash);
#else
    return std::tie(m_local_vid, m_local_eid, m_local_fid, m_global_cid) <
           std::tie(t.m_local_vid, t.m_local_eid, t.m_local_fid, t.m_global_cid);
#endif
}
inline bool Tuple::same_ids(const wmtk::Tuple& t) const
{
    return std::tie(m_local_vid, m_local_eid, m_local_fid, m_global_cid) ==
           std::tie(t.m_local_vid, t.m_local_eid, t.m_local_fid, t.m_global_cid);
}

inline bool Tuple::is_null() const
{
    return m_global_cid == -1;
}

#if defined(WMTK_ENABLE_HASH_UPDATE) 
inline Tuple Tuple::with_updated_hash(int64_t new_hash) const
{
    return Tuple(m_local_vid, m_local_eid, m_local_fid, m_global_cid, new_hash);
}
#endif

inline int8_t Tuple::local_vid() const
{
    return m_local_vid;
}

inline int8_t Tuple::local_eid() const
{
    return m_local_eid;
}

inline int8_t Tuple::local_fid() const
{
    return m_local_fid;
}

} // namespace wmtk
