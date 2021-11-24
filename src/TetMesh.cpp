//
// Created by Yixin Hu on 11/3/21.
//

#include <wmtk/TetMesh.h>
#include <wmtk/Logger.hpp>

int wmtk::TetMesh::find_next_empty_slot_t()
{
	for (int i = m_t_empty_slot; i < m_tet_connectivity.size(); i++)
	{
		if (m_tet_connectivity[i].m_is_removed)
		{
			m_t_empty_slot = i + 1;
			return i;
		}
	}
	m_tet_connectivity.emplace_back();
	return m_tet_connectivity.size() - 1;
}

int wmtk::TetMesh::find_next_empty_slot_v()
{
	for (int i = m_v_empty_slot; i < m_vertex_connectivity.size(); i++)
	{
		if (m_vertex_connectivity[i].m_is_removed)
		{
			m_v_empty_slot = i + 1;
			return i;
		}
	}
	m_vertex_connectivity.emplace_back();
	return m_vertex_connectivity.size() - 1;
}

bool wmtk::TetMesh::split_edge(const Tuple &loc0, std::vector<Tuple>& new_edges){
	if (!split_before(loc0))
		return false;

	// backup of everything
	auto loc1 = loc0;
	int v1_id = loc1.get_vid();
	auto loc2 = loc1.switch_vertex(*this);
	int v2_id = loc2.get_vid();
//    cout<<v1_id<<" "<<v2_id<<endl;
//    loc1.print_info();
//    loc2.print_info();
	//
	auto n12_t_ids = set_intersection(m_vertex_connectivity[v1_id].m_conn_tets,
									  m_vertex_connectivity[v2_id].m_conn_tets);
	std::vector<size_t> n12_v_ids;
	for (size_t t_id : n12_t_ids)
	{
		for (int j = 0; j < 4; j++)
			n12_v_ids.push_back(m_tet_connectivity[t_id][j]);
	}
	vector_unique(n12_v_ids);
	std::vector<std::pair<size_t, TetrahedronConnectivity>> old_tets(n12_t_ids.size());
	std::vector<std::pair<size_t, VertexConnectivity>> old_vertices(n12_v_ids.size());
	for (size_t t_id : n12_t_ids)
		old_tets.push_back(std::make_pair(t_id, m_tet_connectivity[t_id]));
	for (size_t v_id : n12_v_ids)
		old_vertices.push_back(std::make_pair(v_id, m_vertex_connectivity[v_id]));

	// update connectivity
	int v_id = find_next_empty_slot_v();
	std::vector<size_t> new_t_ids;
	for (size_t t_id : n12_t_ids)
	{
		size_t new_t_id = find_next_empty_slot_t();
		new_t_ids.push_back(new_t_id);
		//
		int j = m_tet_connectivity[t_id].find(v1_id);
		m_tet_connectivity[new_t_id] = m_tet_connectivity[t_id];
		m_tet_connectivity[new_t_id][j] = v_id;
		//
		m_vertex_connectivity[v_id].m_conn_tets.push_back(t_id);
		m_vertex_connectivity[v_id].m_conn_tets.push_back(new_t_id);
		//
		for (int j = 0; j < 4; j++)
		{
			if (m_tet_connectivity[t_id][j] != v1_id && m_tet_connectivity[t_id][j] != v2_id)
				m_vertex_connectivity[m_tet_connectivity[t_id][j]].m_conn_tets.push_back(new_t_id);
		}
		//
		j = m_tet_connectivity[t_id].find(v2_id);
		m_tet_connectivity[t_id][j] = v_id;
		//
		vector_erase(m_vertex_connectivity[v2_id].m_conn_tets, t_id);
        m_vertex_connectivity[v2_id].m_conn_tets.push_back(new_t_id);
	}

	// possibly call the resize_attributes
	resize_attributes(m_vertex_connectivity.size(), m_tet_connectivity.size() * 6,
					  m_tet_connectivity.size() * 4, m_tet_connectivity.size());

    std::vector<Tuple> locs;
    for (size_t t_id : n12_t_ids){
        locs.push_back(Tuple(v_id,0,0,t_id));
    }
    for(size_t t_id: new_t_ids){
        locs.push_back(Tuple(v_id,0,0,t_id));
    }
	if (!split_after(locs)) {
        m_vertex_connectivity[v_id].m_is_removed = true;
        for (int t_id: new_t_ids)
            m_tet_connectivity[t_id].m_is_removed = true;
        //
        for (int i = 0; i < old_tets.size(); i++) {
            int t_id = old_tets[i].first;
            m_tet_connectivity[t_id] = old_tets[i].second;
        }
        for (int i = 0; i < old_vertices.size(); i++) {
            int v_id = old_vertices[i].first;
            m_vertex_connectivity[v_id] = old_vertices[i].second;
        }

        return false;
    }

	// call invariants on all entities
	if (false) // if any invariant fails
	{
		// undo changes
		return false;
	}

    //new_edges
    for (size_t t_id : n12_t_ids){
        for(int j=0;j<6;j++){
            Tuple loc;
            int eid = j;
            int vid = m_tet_connectivity[t_id][local_edges[j][0]];
            int fid = map_edge2face[eid];
            new_edges.push_back(Tuple(vid, eid, fid, t_id));
        }
    }
    for(size_t t_id: new_t_ids){
        for(int j=0;j<6;j++){
            Tuple loc;
            int eid = j;
            int vid = m_tet_connectivity[t_id][local_edges[j][0]];
            int fid = map_edge2face[eid];
            new_edges.push_back(Tuple(vid, eid, fid, t_id));
        }
    }
    std::sort(new_edges.begin(), new_edges.end(), [&](const Tuple &a, const Tuple &b) {
        return a.compare_edges(*this, b) < 0;
    });
    new_edges.erase(std::unique(new_edges.begin(), new_edges.end(), [&](const Tuple &a, const Tuple &b) {
        return a.compare_edges(*this, b) == 0;
    }), new_edges.end());

    ///update timestamps
    timestamp++;//todo: thread
    for (size_t t_id : n12_t_ids)
        m_tet_connectivity[t_id].set_version_number(timestamp);
    for(size_t t_id: new_t_ids)
        m_tet_connectivity[t_id].set_version_number(timestamp);
    for(auto& new_loc: new_edges)//update edge timestamp from tets
        new_loc.update_version_number(*this);

    return true;
}
