#	pragma once

#	include "fastpathfinder/config.h"

namespace fastpathfinder
{	
	struct graph_edge
	{
		struct graph_node * to;
		uint32_t weight;
	};

	typedef FASTPATHFINDER_VECTOR<graph_edge> vector_graph_edge;

	struct graph_node
	{		 
		vector_graph_edge edges;
		bool block;

		uint32_t weight;
	};

	typedef FASTPATHFINDER_VECTOR<graph_node *> vector_graph_node;

	class graph
	{
	public:
		void addNode( graph_node * _node, bool _block )
		{
			_node->block = _block;
			_node->weight = (uint32_t)-1;
			
			m_nodes.push_back( _node );
		}

		bool addEdge( graph_node * _from, graph_node * _to, uint32_t _weight )
		{
			if( _from->weight != (uint32_t)-1 )
			{
				return false;
			}

			if( _to->weight != (uint32_t)-1 )
			{
				return false;
			}

			if( _weight == 0 )
			{
				return false;
			}

			graph_edge edge;
			edge.to = _to;
			edge.weight = _weight;

			_from->edges.push_back( edge );

			return true;
		}

		bool addEdge2( graph_node * _from, graph_node * _to, uint32_t _weight )
		{
			if( this->addEdge( _from, _to, _weight ) == false )
			{
				return false;
			}
			
			if( this->addEdge( _to, _from, _weight ) == false )
			{
				return false;
			}

			return true;
		}

		void blockNode( graph_node * _node, bool _block )
		{
			_node->block = _block;
		}
				
	public:
		void getPath( graph_node * _from, graph_node * _to, vector_graph_node & _path )
		{
			this->clearWeight_();

			this->wave_( _to, _from, 0 );

			this->makePath_( _from, _path );
		}

	protected:
		void clearWeight_()
		{
			for( vector_graph_node::iterator
				it = m_nodes.begin(),
				it_end = m_nodes.end();
			it != it_end;
			++it )
			{
				graph_node * node = *it;

				node->weight = (uint32_t)-1;
			}
		}

		void wave_( graph_node * _from, graph_node * _to, uint32_t _weight )
		{
			_from->weight = _weight;

			for( vector_graph_edge::iterator
				it = _from->edges.begin(),
				it_end = _from->edges.end();
			it != it_end;
			++it )
			{
				graph_edge & edge = *it;

				if( edge.to->block == true )
				{
					continue;
				}

				uint32_t step_weight = _from->weight + edge.weight;

				if( step_weight > edge.to->weight )
				{
					continue;
				}
				
				this->wave_( edge.to, _to, step_weight );
			}
		}

		void makePath_( graph_node * _from, vector_graph_node & _path )
		{
			_path.push_back( _from );

			for( vector_graph_edge::iterator
				it = _from->edges.begin(),
				it_end = _from->edges.end();
			it != it_end;
			++it )
			{
				graph_edge & edge = *it;

				uint32_t step_weight = _from->weight - edge.weight;

				if( step_weight != edge.to->weight )
				{
					continue;
				}

				this->makePath_( edge.to, _path );
			}
		}
		
	protected:
		vector_graph_node m_nodes;
	};
}