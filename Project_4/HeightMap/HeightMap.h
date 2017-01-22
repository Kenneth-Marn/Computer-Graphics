
/**
 This class draws a table by scaling and moving blocks.
 The table top is the plane y = 0
*/
#include <vector>
#include <string>

struct imgInfo{
    int width, height, channels;
    std::string TexNames;

};
class HeightMap
{
    private:
        int width, height, channels;
        float numeratorW, numeratorH;
        unsigned char *IMGdata;
        unsigned int vaoHeightHandle;
        std::vector<imgInfo> Images;



    public:
        void readingIMG();
        int createHeightMapVAO(int programID);
        HeightMap( int id);
        void render(int programID);
        void genTextures(int programID);
};
