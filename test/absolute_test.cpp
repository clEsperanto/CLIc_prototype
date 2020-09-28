/**
 * Author: Stephane Rigaud - @strigaud 
 */

#include <random>
#include "tiffreader.h"
#include "tiffwriter.h"

#include "CLE.h"

/**
 * Main test function
 *
 */
int main(int argc, char **argv)
{
    // Initialise random input and valid output.
    unsigned int width (10), height (10), depth (10);
    float* input_data = new float[width*height*depth];
    float* valid_data = new float[width*height*depth];
    for (size_t i = 0; i < width*height*depth; i++)
    {
        if (i % 2 == 0)
        {
            input_data[i] = -1;
        }
        else
        {
            input_data[i] = 1;
        }
        valid_data[i] = 1;
    }
    Image<float> input_img (input_data, width, height, depth, "float");

    // Initialise GPU information.
    cle::GPU gpu;
    cle::CLE cle(gpu);
    
    // Initialise device memory and push from host
    cle::Buffer gpuInput = cle.Push<float>(input_img);
    cle::Buffer gpuOutput = cle.Create<float>(input_img);

    // Call kernel
    cle.Absolute(gpuInput, gpuOutput);  

    // pull device memory to host
    Image<float> output_img = cle.Pull<float>(gpuOutput);    

    // Verify output
    float difference = 0;
    for (size_t i = 0; i < width*height*depth; i++)
    {
        difference += std::abs(valid_data[i] - output_img.GetData()[i]);
    }
    if (difference > std::numeric_limits<float>::epsilon())
    {
        std::cout << "Test failled, cumulated absolute difference " << difference << " > CPU epsilon (" << std::numeric_limits<float>::epsilon() << ")" << std::endl;
        return EXIT_FAILURE;
    }

    // That's all folks!
    std::cout << "Test succeded!"<< std::endl;
    return EXIT_SUCCESS;
}