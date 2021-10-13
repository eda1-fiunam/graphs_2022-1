/*Copyright (C) 
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * 2021 - francisco dot rodriguez at ingenieria dot unam dot mx
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>


// 20/06/21:
// Esta versión no borra elementos
// Esta versión no modifica los datos originales

#ifndef DBG_HELP
#define DBG_HELP 0
#endif  

#if DBG_HELP > 0
#define DBG_PRINT( ... ) do{ fprintf( stderr, "DBG:" __VA_ARGS__ ); } while( 0 )
#else
#define DBG_PRINT( ... ) ;
#endif  

/**
 * @brief Es la información real.
 */
typedef struct 
{
   int val;   ///< campo de indexado (key) para los vértices
   char name; ///< una letra (por el momento)
} Data;


//----------------------------------------------------------------------
// Vertex stuff: 
//----------------------------------------------------------------------

/**
 * @brief Declara lo que es un nodo (lo usa la clase Vertex).
 */
typedef struct Node_
{
   size_t index;       ///< índice del elemento en la lista de vértices
   struct Node_ *next; ///< índice al nodo siguiente en la lista simplemente enlazada
} Node;

/**
 * @brief Mantiene juntas la llave (que se usa a lo largo de todas las funciones
 * de los grafos) y el índice de la misma en la tabla de datos.
 */
typedef struct
{
   int    key;      ///< campo de indexado. En este ejemplo Data.val
   size_t data_idx; ///< índice del elemento en la tabla de datos
} Map;

/**
 * @brief Declara lo que es un vértice.
 */
typedef struct
{
   Map   map;       ///< Par <key, value>: <Data.val, index (a la tabla de datos)>
   Node* neighbors; ///< apunta al primer vecino, ¡no debe perderse!
   Node* cursor;    ///< nodo libre
} Vertex;

/**
 * @brief Devuelve el índice del vecino (correspondiente a la lista de vértices) al 
 * que apunta el |cursor| en la lista de vecinos.
 *
 * @param v El vértice base (del cual queremos conocer el índice de su vecino).
 *
 * @return El índice del vecino en la lista de vértices.
 *
 * @pre El cursor debe apuntar a un nodo válido en la lista de vecinos.
 *
 * Ejemplo
 * @code
   Vertex* v = Graph_GetVertexByKey( grafo, 100 );

   for( Vertex_Start( v ); !Vertex_End( v ); Vertex_Next( v ) )
   {
      size_t index = Vertex_Get( v );

      printf( "[Index=%ld, [Val=%d, Name=%c] ], ", 
         index, 
         data[ index ].val,
         data[ index ].name );
   }
   @endcode

   @note Esta función debe utilizarse únicamente cuando se recorra el grafo con las funciones Vertex_Start(), Vertex_End() y Vertex_Next().
 */
size_t Vertex_Get( const Vertex* v )
{
   assert( v->cursor );

   return v->cursor->index;
}

/**
 * @brief Hace que cursor libre apunte al inicio de la lista de vecinos. Se debe
 * de llamar siempre que se vaya a iniciar un recorrido de dicha lista.
 *
 * @param v El vértice de trabajo (es decir, el vértice del cual queremos obtener 
 * la lista de vecinos).
 */
void Vertex_Start( Vertex* v )
{
   assert( v );

   v->cursor = v->neighbors;
}

/**
 * @brief Mueve al cursor libre un nodo adelante.
 *
 * @param v El vértice de trabajo.
 *
 * @pre El cursor apunta a un nodo válido.
 * @post El cursor se movió un elemento a la derecha en la lista de vecinos.
 */
void Vertex_Next( Vertex* v )
{
   assert( v->cursor );

   v->cursor = v->cursor->next;
}

/**
 * @brief Indica si se alcanzó el final de la lista de vecinos.
 *
 * @param v El vértice de trabajo.
 *
 * @return true si se alcanazó el final de la lista; false en cualquier otro
 * caso.
 */
bool Vertex_End( const Vertex* v )
{
   return v->cursor == NULL;
}

/**
 * @brief Devuelve la llave asociada al vértice indicado.
 *
 * @param v El vértice de trabajo.
 *
 * @return La llave asociada al vértice indicado.
 */
int Vertex_GetKey( Vertex* v )
{
   // para terminar
}

/**
 * @brief Devuelve el índice en la tabla de datos asociado al vértice indicado.
 *
 * @param v El vértice de trabajo.
 *
 * @return El índice en la tabla de datos asociado al vértice indicado.
 */
size_t Vertex_GetDataIndex( Vertex* v )
{
   return v->map.data_idx;
}



//----------------------------------------------------------------------
// Graph stuff: 
//----------------------------------------------------------------------

/** Tipo del grafo.
 */
typedef enum 
{ 
   UNDIRECTED, ///< grafo no dirigido
   DIRECTED    ///< grafo dirigido (digraph)
} eGraphType; 

/**
 * @brief Declara lo que es un grafo.
 */
typedef struct
{
   Vertex* vertices; ///< Lista de vértices
   size_t size;      ///< Tamaño de la lista de vértices

   /**
    * Número de vértices actualmente en el grafo. 
    * Como esta versión no borra vértices, lo podemos usar como índice en la
    * función de inserción
    */
   size_t len;  

   eGraphType type; ///< tipo del grafo, UNDIRECTED o DIRECTED
} Graph;

/**
 * @brief Crea un nuevo grafo.
 *
 * @param size Número de vértices que tendrá el grafo. Este valor no se puede
 * cambiar luego de haberlo creado.
 *
 * @return Un nuevo grafo.
 *
 * @pre El número de elementos es mayor que 0.
 */
Graph* Graph_New( size_t size, eGraphType type )
{
   assert( size > 0 );

   Graph* g = (Graph*) malloc( sizeof( Graph ) );
   if( g ){
      g->size = size;
      g->len = 0;
      g->type = type;

      g->vertices = (Vertex*) calloc( size, sizeof( Vertex ) );
      // crea un arreglo para guardar los vértices


      // La siguiente inicialización NO ES NECESARIA, dado que calloc() ya se
      // encargó de eso; lo hice por si alguno de los campos debe ser puesto a
      // un valor diferente de cero:
      if( g->vertices ){
         
         Vertex* vertices = g->vertices;
         // para simplificar la notación. 
         // La variable |vertices| sólo existe dentro de este if.

         for( size_t i = 0; i < g->size; ++i ){
            vertices[ i ].map.key = 0;
            vertices[ i ].map.data_idx = 0;
            vertices[ i ].neighbors = NULL;
         }
      } 

      // error de memoria para g->vertices:
      else{ 
         free( g );
         g = NULL;
      }
   }

   return g;
   // el cliente es responsable de verificar que el grafo se creó correctamente
}

void Graph_Delete( Graph** g )
{
   assert( *g );

   Graph* graph = *g;
   // para simplificar la notación 

   // 1. Borramos la verticesa enlazada de cada vértice:
   for( size_t i = 0; i < graph->size; ++i ){

      Vertex* vertex = &graph->vertices[ i ];
      // para simplificar la notación. 
      // La variable |vertex| sólo existe dentro de este for.

      Node* node = vertex->neighbors;
      while( node ){
         Node* tmp = node->next;
         free( node );
         node = tmp;
      }
   }

   // 2. Borramos el arreglo dinámico de vértices:
   free( graph->vertices );

   // 3. Borramos al objeto grafo:
   free( graph );

   // 4. Hacemos que el grafo apunte a nada (para control de daños):
   *g = NULL;
}

/**
 * @brief Imprime un reporte del grafo
 *
 * @param g     El grafo.
 * @param depth Cuán detallado deberá ser el reporte (0: lo mínimo)
 */
void Graph_Print( Graph* g, int depth )
{
   for( size_t i = 0; i < g->len; ++i ){

      Vertex* vertex = &g->vertices[ i ];
      // para simplificar la notación. 

      printf( "\n=== Vertex[ %ld ] ===\n", i );
      printf( "<map.key:%d, map.data_idx:%ld>\n", vertex->map.key, vertex->map.data_idx );

      // LEVEL 0:
      printf( vertex->neighbors ? "Has neighbors\n" : "Has no neighbors\n" );

      // LEVEL 1:
      if( depth > 0 ){

         for( Node* node = vertex->neighbors; node != NULL; node = node->next ){

            DBG_PRINT( "Print():(Node:%p, (*Node.index:%ld, *Node.next:%p))\n", node, node->index, node->next );
            
            printf( " %d ", g->vertices[ node->index ].map.key );

            // LEVEL 2:
            if( depth > 1 ){
               printf( "(Node:%p) ", node );
            }

            printf( "->" );
         }
         if( vertex->neighbors ) printf( " Nil\n" );
      }
   }
   printf( "\n" );
}

/**
 * @brief Crea un vértice a partir de los datos reales.
 *
 * @param g     El grafo.
 * @param key   Es el campo de indexado en los datos.
 * @param index Es el índice de los datos en la tabla de datos.
 */
void Graph_AddVertex( Graph* g, int key, size_t index )
{
   assert( g->len < g->size );

   Vertex* vertex = &g->vertices[ g->len ];
   // para simplificar la notación 

   vertex->map.key      = key;
   vertex->map.data_idx = index;
   vertex->neighbors    = NULL;

   ++g->len;
}

size_t Graph_GetSize( Graph* g )
{
   return g->size;
}


// vertices: lista de vértices
// size: número de elementos en la lista de vértices
// key: valor a buscar
// ret: el índice donde está la primer coincidencia; -1 si no se encontró
static int find( Vertex* vertices, size_t size, int key )
{
   for( size_t i = 0; i < size; ++i ){

      if( vertices[ i ].map.key == key ) return i;
   }

   return -1;
}

// busca si el índice (a la tabla de vértices) ya está en un vértice
bool find_index_in_vertex( Vertex* vertex, int index )
{
   for( Node* node = vertex->neighbors; node != NULL; node = node->next ){
      if( node->index == index ) return true;
   }
   return false;
}

// vertex: vértice de trabajo
// index: índice en la lista de vértices del vértice vecino que está por insertarse
static void insert( Vertex* vertex, int index )
{
   if( find_index_in_vertex( vertex, index ) == false )
   {
      // Crea al nodo:
      Node* n = (Node*) calloc( 1, sizeof( Node ) );
      assert( n );

      // Inserta al nodo al frente de la lista
      n->index = index;
      n->next = vertex->neighbors;
      vertex->neighbors = n;

      DBG_PRINT( "insert():Inserting the neighbor with (idx:%d, Node:%p)\n", index, n );
   } 
   else DBG_PRINT( "insert: duplicated index\n" );

}

/**
 * @brief Inserta una relación de adyacencia del vértice |start| hacia el vértice |finish|.
 *
 * @param g     El grafo.
 * @param start Vértice de salida.
 * @param finish Vertice de llegada.
 *
 * @return false si uno o ambos vértices no existen; true si la relación se creó con éxito.
 *
 * @pre El grafo no puede estar vacío.
 */
bool Graph_AddEdge( Graph* g, int start, int finish )
{
   assert( g->len > 0 );

   // obtenemos los índices correspondientes:
   int start_idx = find( g->vertices, g->size, start );
   int finish_idx = find( g->vertices, g->size, finish );

//   DBG_PRINT( "AddEdge(): start_idx=%d, finish_idx=%d\n", start_idx, finish_idx );
   DBG_PRINT( "AddEdge(): from:%d (with index:%d), to:%d (with index:%d)\n", start, start_idx, finish, finish_idx );

   if( start_idx == -1 || finish_idx == -1 ) return false;
   // uno o ambos vértices no existen

   insert( &g->vertices[ start_idx ], finish_idx );
   // insertamos la arista start-finish

   if( g->type == UNDIRECTED ) insert( &g->vertices[ finish_idx ], start_idx );
   // si el grafo no es dirigido, entonces insertamos la arista finish-start

   return true;
}

/**
 * @brief Devuelve el nodo que apunta al inicio de la lista de vecinos.
 *
 * @param g     El grafo.
 * @param key   El criterio de búsqueda
 *
 * @return      El nodo que apunta a la lista de vecinos. Si no los tuviera,
 *              devuelve NULL
 */
Node* Graph_GetNeighborsByKey( Graph* g, int key )
{
   for( size_t i = 0; i < g->len; ++i ){

      if( g->vertices[ i ].map.key == key ) return g->vertices[ i ].neighbors;
   }

   return NULL;
}

Vertex* Graph_GetVertexByKey( Graph* g, int key )
{
   assert( g );

   for( size_t i = 0; i < g->len; ++i ){

      if( g->vertices[ i ].map.key == key ) return &g->vertices[ i ];
   }

   return NULL;
}

Vertex* Graph_GetVertexByIndex( Graph* g, size_t index )
{
   assert( g );
   assert( index < g->len );

   return &g->vertices[ index ];
}


/**
 * @brief Devuelve el índice en la lista de vértices del vértice indicado.
 *
 * @param g El grafo.
 * @param v El vértice del cual queremos obtener el índice.
 *
 * @return El índice en la lista de vértices del vértice v.
 *
 * @pre Ambos, el grafo y el vértice deben existir.
 */
size_t Graph_GetIndex( Graph* g, Vertex* v )
{
   assert( g );
   assert( v );

   size_t index;
   for( index = 0; index < g->len; ++index ){
      if( g->vertices[ index ].map.key == v->map.key ) break;
   }

   assert( index < g->len );

   return index;
}

size_t Graph_GetLen( Graph* g )
{
   return g->len;
}



//----------------------------------------------------------------------
// Driver program 
//----------------------------------------------------------------------
#define MAX_DATA 10

int main()
{
   // Inicializa la tabla de datos. <1,A>, <2,B>, ...
   Data data[ MAX_DATA ];
   char name = 'A';
   for( size_t i = 1; i <= MAX_DATA; ++i, ++name) {
      data[i-1].val = i * 1;
      data[i-1].name = name;
   }

   Graph* grafo = Graph_New( 
         MAX_DATA,   // cantidad máxima de nodos
         DIRECTED ); // será un grafo dirigido

   // crea los vértices:
   for( size_t i = 0; i < MAX_DATA; ++i )
   {

      Graph_AddVertex( 
            grafo,         // el grafo de trabajo
            data[ i ].val, // el campo de indexado en los datos
            i );           // índice de los datos en la tabla de datos
   }

   // crea las conexiones entre vértices:
   Graph_AddEdge( grafo, 1, 2 );
   Graph_AddEdge( grafo, 2, 6 );
   Graph_AddEdge( grafo, 4, 5 );
   Graph_AddEdge( grafo, 5, 1 );
   Graph_AddEdge( grafo, 5, 8 );
   Graph_AddEdge( grafo, 6, 7 );
   Graph_AddEdge( grafo, 6, 9 );
   Graph_AddEdge( grafo, 7, 3 );
   Graph_AddEdge( grafo, 7, 10 );
   Graph_AddEdge( grafo, 8, 4 );
   Graph_AddEdge( grafo, 9, 8 );

   Graph_Print( grafo, 2 );

   Graph_Delete( &grafo );
   assert( grafo == NULL );
}
