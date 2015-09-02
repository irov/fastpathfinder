#	pragma once

#	include "fastpathfinder/config.h"

namespace fastpathfinder
{	
	static const uint32_t FASTPATHFINDER_INVALID_WEIGHT = (uint32_t)-1;

	struct graph_edge
	{
		struct graph_node * to;		
		uint32_t mask;
		uint32_t weight;
	};

	typedef FASTPATHFINDER_VECTOR<graph_edge> vector_graph_edge;

	struct graph_node
	{		 
		vector_graph_edge edges;
		uint32_t block;

		uint32_t weight;
	};

	typedef FASTPATHFINDER_VECTOR<graph_node *> vector_graph_node;

	class graph
	{
	public:
		void addNode( graph_node * _node, uint32_t _block )
		{
			_node->block = _block;
			_node->weight = FASTPATHFINDER_INVALID_WEIGHT;
			
			m_nodes.push_back( _node );
		}

		bool addEdge( graph_node * _from, graph_node * _to, uint32_t _mask, uint32_t _weight )
		{
			if( _from->weight != FASTPATHFINDER_INVALID_WEIGHT )
			{
				return false;
			}

			if( _to->weight != FASTPATHFINDER_INVALID_WEIGHT )
			{
				return false;
			}

			if( _weight == 0 )
			{
				return false;
			}

			graph_edge edge;
			edge.to = _to;
			edge.mask = _mask;
			edge.weight = _weight;

			_from->edges.push_back( edge );

			return true;
		}

		bool addEdge2( graph_node * _from, graph_node * _to, uint32_t _mask, uint32_t _weight )
		{
			if( this->addEdge( _from, _to, _mask, _weight ) == false )
			{
				return false;
			}
			
			if( this->addEdge( _to, _from, _mask, _weight ) == false )
			{
				return false;
			}

			return true;
		}

		void setBlock( graph_node * _node, uint32_t _block )
		{
			_node->block = _block;
		}

		uint32_t getBlock( graph_node * _node ) const
		{
			uint32_t block = _node->block;

			return block;
		}
				
	public:
		bool hasPath( graph_node * _from, graph_node * _to, uint32_t _mask )
		{
			if( _from == _to )
			{
				return true;
			}

			this->clearWeight_();

			this->wave_( _to, _from, _mask, 0 );

			if( _from->weight == FASTPATHFINDER_INVALID_WEIGHT )
			{
				return false;
			}

			return true;
		}

		bool getPath( graph_node * _from, graph_node * _to, uint32_t _mask, vector_graph_node & _path )
		{
			if( _from == _to )
			{
				return true;
			}

			this->clearWeight_();

			this->wave_( _to, _from, _mask, 0 );

			if( _from->weight == FASTPATHFINDER_INVALID_WEIGHT )
			{
				return false;
			}

			this->makePath_( _from, _path );

			return true;
		}

		uint32_t getPathWeight( graph_node * _from, graph_node * _to, uint32_t _mask )
		{
			if( _from == _to )
			{
				return 0;
			}

			this->clearWeight_();

			this->wave_( _to, _from, _mask, 0 );

			if( _from->weight == FASTPATHFINDER_INVALID_WEIGHT )
			{
				return 0;
			}

			uint32_t weight = this->makeWeight_( _from, 0 );

			return weight;
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

				node->weight = FASTPATHFINDER_INVALID_WEIGHT;
			}
		}

		void wave_( graph_node * _from, graph_node * _to, uint32_t _mask, uint32_t _weight )
		{
			_from->weight = _weight;

			for( vector_graph_edge::iterator
				it = _from->edges.begin(),
				it_end = _from->edges.end();
			it != it_end;
			++it )
			{
				graph_edge & edge = *it;

				if( (edge.mask & _mask) != edge.mask )
				{
					continue;
				}

				uint32_t step_weight = _from->weight + edge.weight;

				if( step_weight > edge.to->weight )
				{
					continue;
				}

				if( edge.to->block != 0 )
				{
					if( edge.to == _to )
					{
						_to->weight = _from->weight + edge.weight;
					}

					continue;
				}
				
				this->wave_( edge.to, _to, _mask, step_weight );
			}
		}

		void makePath_( graph_node * _from, vector_graph_node & _path )
		{
			_path.push_back( _from );

			if( _from->weight == 0 )
			{
				return;
			}

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

				break;
			}
		}

		uint32_t makeWeight_( graph_node * _from, uint32_t _weight )
		{
			if( _from->weight == 0 )
			{
				return _weight;
			}

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

				uint32_t total_weight = this->makeWeight_( edge.to, _weight + edge.weight );

				return total_weight;
			}

			return _weight;
		}
		
	protected:
		vector_graph_node m_nodes;
	};
}