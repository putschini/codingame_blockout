#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

class Shape {

    vector <vector< vector <int> > > shape_;
    int width_;
    int depth_;
    int height_;

public:

    Shape( string shape, int width, int height, int depth )
      : width_(width), height_(height), depth_(depth) {

        for( int i = 0; i < height; i++) {
            shape_.push_back( vector<vector<int>>() );
            for ( int x = 0; x < width; x++ ) {
                shape_[i].push_back( vector<int>() );
                for ( int y = 0; y < depth; y++ ) {
                    if(shape[ (i * width * depth) + (y * width) + x ] == '.' )
                        shape_[i][x].push_back(0);
                    else
                        shape_[i][x].push_back(1);
                }
            }
        }
    }

    vector<vector<int>> get_layer( int i ) {
        return shape_[i];
    }

    int height( ) {
        return height_;
    }

    int width() {
        return width_;
    }

    int depth() {
        return depth_;
    }
};



class Block {

    vector<Shape> shapes_;
    int block_count_;

public:

    Block( int block_count )
      : block_count_(block_count) {
    }

    void add_shape( int width, int height, int depth, string shape ) {
        shapes_.push_back( Shape(shape, width, height, depth) );
    }

    vector<Shape> get_shapes( ) {
        return shapes_;
    }

    Shape get_shape( int i ) {
        return shapes_[i];
    }

    int block_count( ) {
        return block_count_;
    }
};


class Cell 
{
    int value_;
    bool blocked_;
public:

    Cell()
      : value_(0), blocked_(false) {
    }

    Cell( int v )
      : value_(v), blocked_(false) {
    }

    bool is_valid( ) {
        return !blocked_ && value_ == 0;
    }

    void set_value(int value) {
        value_ = value;
    }

    void block( ) {
        blocked_ = true;
    }

    int value() {
        return value_;
    }
};


class Layer {
    vector< vector <Cell> > layer_;

    int capacity_;
    int width_;
    int depth_;
public:    
    Layer( int width, int depth )
      : width_(width), depth_(depth), capacity_(width * depth) {

        for( int i = 0; i < width_; i++ ) {
            vector<Cell> line;
            for(int j = 0; j < depth_; j++ ) {
                line.push_back( Cell());
            }
            layer_.push_back(line);
        }
    }

    bool is_full() {
        return capacity_ == 0;
    }

    int capacity() {
        return capacity_;
    }

    int fit_in_line( int line_index, std::vector<int> shape_line, int offset ) {
        int size = shape_line.size( );
        int count = 0;
        for( int i = offset; i < depth_; i++ ) {
            if (!layer_[line_index][i].is_valid() && shape_line[count] == 1 ) {
                count = 0;
                continue;
            }
            count++;
            if(count == size)
                return i - (count - 1);
        }
        return -1;
    }

    pair<int, int> fit( Shape shape ) {
        vector<vector<int>> layer = shape.get_layer(0);
        int shape_line = 0;
        int layer_z_offset = 0;

        for( int i = 0; i < width_; i++ ) {
            int z = fit_in_line(i, layer[shape_line], layer_z_offset );
            if ( z == -1 ) {
                i -= shape_line;
                shape_line = 0;
                layer_z_offset = 0;
                continue;
            }
            shape_line++;
            if ( shape_line == layer.size()) {
                return pair<int, int>( i - (shape_line - 1), z);
            }
            if (layer_z_offset == 0)
                layer_z_offset = z;
        }

        return pair<int, int>(-1, -1);
    }

    void update_layer_cells( vector<vector<int>> shape_layer, int x, int z ) {
        for (int i = 0; i < shape_layer.size(); i++) {
            for (int j = 0; j < shape_layer[i].size(); j++) {
                if (shape_layer[i][j] == 1) {
                    layer_[x + i][z + j].set_value(1);
                    capacity_--;
                }
            }
        }
    }

    void update_blocked_cells( vector<vector<int>> shape_layer, int x, int z ) {
        for (int i = 0; i < shape_layer.size(); i ++) {
            for (int j = 0; j < shape_layer[i].size(); j++) {
                if (shape_layer[i][j] == 1){
                    layer_[x + i][z + j].block();
                }
            }
        }
    }


    void update_blocked_cells( vector<vector<Cell>> above_layer ) {
        for (int i = 0; i < above_layer.size(); i ++) {
            for (int j = 0; j < above_layer[i].size(); j++) {
                if (!above_layer[i][j].is_valid()) {
                    layer_[i][j].block();
                }
            }
        }
    }

    int get_added_cells_score( vector<vector<int>> shape_layer, int x, int z ) {
        int score = 0;
        for (int i = 0; i < shape_layer.size(); i ++) {
            for (int j = 0; j < shape_layer[i].size(); j++) {
                if (shape_layer[i][j] == 1){
                    score++;
                }
            }
        }

        if (x + shape_layer.size() == layer_.size())
            score += 5;
        if (z + shape_layer[0].size() == layer_[0].size())
            score += 5;

        return score;
    }

    int get_blocked_cells_score( Shape shape, int x, int z, int layer_index ) {
        int score = 0;

        for (int i = 0; i < shape.width(); i ++) {
            for (int j = 0; j < shape.depth(); j++) {
                bool will_block = false;
                for (int y = layer_index; y < shape.height(); y++) {
                    vector<vector<int>> shape_layer = shape.get_layer(y);
                    // Skip if will block itself
                    if (y > 0 ) {
                        vector<vector<int>> lower_shape_layer = shape.get_layer(y - 1);
                        if( lower_shape_layer[i][j] == 1 && shape_layer[i][j] == 1 )
                            break;
                    }

                    if ( layer_[x+i][z+j].is_valid() && shape_layer[i][j] == 1 )
                        will_block = true;
                }
                if (will_block) {
                    score -= 10;
                }
            }
        }


        return score;
    }

    vector< vector <Cell> > layer() {
        return layer_;
    }

};

struct response {
    int x;
    int z;
    int block_index;
    int score;
};

class Pit {

    vector<Layer*> pit_;
    int width_;
    int depth_;
    int height_;

public:
    Pit( int width, int height, int depth )
      : width_(width), height_(height), depth_(depth) {

        pit_.push_back(new Layer(width_, depth_));
    }


    void update_pit( Shape shape, int x, int z, int layer ) {
        for (int i = pit_.size(); i < layer + shape.height(); i++ ) {
            pit_.push_back( new Layer( width_, depth_ ) );
        }

        for (int i = 0; i < shape.height(); i++) {
            pit_[layer + i]->update_layer_cells( shape.get_layer(i), x, z );
            if (i > 0 )
                pit_[layer + i - 1]->update_blocked_cells( shape.get_layer(i), x, z );
        }
        
        remove_full_layers( );
    }

    void remove_full_layers( ) {
        bool line_removed = false;
        for( vector<Layer*>::iterator layer = pit_.begin(); layer != pit_.end(); layer++ ) {
            if( (*layer)->is_full() ) {
                line_removed = true;
                pit_.erase(layer);
            }
        }

        if ( line_removed ) {
            for( int i = pit_.size() - 1; i > 1; i-- ) {
                pit_[i-1]->update_blocked_cells( pit_[i]->layer() );
            }
        }
    }

    int compute_score( Shape shape, int x, int z, int layer ) {
        int score = 0;

        score += pit_[layer]->get_added_cells_score( shape.get_layer(0), x, z );

        if (shape.height() > 1)
            score += pit_[layer]->get_blocked_cells_score(shape, x, z, 1);        

        for (int i = layer - 1; i > 0; i--) {
            score += pit_[i]->get_blocked_cells_score(shape, x, z, 0);
        }

        return score;
    }

    response fit( Block block, int offset = 0 ) {
        response res = response{.x = -1, .z = -1, .block_index = -1, .score = -100};
        vector<Shape> shapes = block.get_shapes();
        bool position_found = false;
        int bottom_layer = 0;

        for( int i = 0; i < shapes.size(); i ++ ) {
            bottom_layer = 0;
            for (Layer* layer : pit_) {
                pair<int, int> position = layer->fit(shapes[i]);
                if(position.first != -1)
                {
                    int new_score = compute_score(block.get_shape( i ), position.first, position.second, bottom_layer);
                    if (new_score > res.score) {
                        res.score = new_score;
                        res.x = position.first;
                        res.z = position.second; 
                        res.block_index = i;
                    }
                }
                bottom_layer++;
            }

        }

        if (res.x == -1) {
            Layer* new_layer = new Layer(width_, depth_);
            pit_.push_back(new_layer);
            return fit(block);
        }

        update_pit( block.get_shape( res.block_index ), res.x, res.z, bottom_layer );

        return res;
    }

    vector<Layer*> get_pit() {
        return pit_;
    }

};

void print_pit( Pit* pit ) {

    for(Layer* layer: pit->get_pit()) {
        vector< vector <Cell>> l = layer->layer();
        for(int x = 0; x < l.size(); x++) {
            for( int y = 0; y < l[0].size(); y++){
                if( l[x][y].value() )
                    cerr << "#";
                else
                    cerr << ".";
            }
            cerr << endl;
        }
        cerr << endl;
    }

}

int main()
{

    Pit* pit = nullptr;

    while (1) {
        int pit_width;
        int pit_height;
        int pit_depth;
        string pit_shape;
        cin >> pit_width >> pit_height >> pit_depth >> pit_shape; cin.ignore();
        if (pit == nullptr) {
            pit = new Pit(pit_width, pit_height, pit_depth);
        }
        response res;
        Block block = Block(-1);
        int block_count;
        cin >> block_count; cin.ignore();
        for (int i = 0; i < block_count; i++) {
            int block_index = 0;
            int width = 0;
            int height = 0;
            int depth = 0;
            string shape;
            cin >> block_index >> width >> height >> depth >> shape; cin.ignore();

            if (block.block_count() == -1 ){
                block = Block(block_count);
            }
            block.add_shape( width, height, depth, shape );
        }

        res = pit->fit(block);
//        print_pit(pit);
        cout << "" << res.block_index << " " << res.x << " " << res.z << endl;
    }
}