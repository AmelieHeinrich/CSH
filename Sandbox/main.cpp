#include <csh/csh.h>
#include <iostream>

#define NUM_ELEMENTS 1024

void Addition_Example()
{
    struct Addition
    {
        int value;
    };

    Addition addBuffer0[NUM_ELEMENTS];
    Addition addBuffer1[NUM_ELEMENTS];

    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        addBuffer0[i].value = i;
        addBuffer1[i].value = i;
    }

    // Create the compute shader
    CSH_ComputeShader* addShader = new CSH_ComputeShader(L"shaders/Add.hlsl");

    // Create the buffers to use in the compute shader
    CSH_Buffer* buffer0 = new CSH_Buffer(sizeof(Addition), NUM_ELEMENTS, &addBuffer0[0]);
    CSH_Buffer* buffer1 = new CSH_Buffer(sizeof(Addition), NUM_ELEMENTS, &addBuffer1[0]);
    CSH_Buffer* outputBuffer = new CSH_Buffer(sizeof(Addition), 1024, nullptr);
    buffer0->BuildSRV();
    buffer1->BuildSRV();
    outputBuffer->BuildUAV();
    //

    // Run the compute shader
    addShader->Bind();
    buffer0->BindSRV(0);
    buffer1->BindSRV(1);
    outputBuffer->BindUAV(0);

    addShader->Dispatch(NUM_ELEMENTS, 1, 1);

    outputBuffer->UnbindUAV(0);
    buffer1->UnbindSRV(1);
    buffer0->UnbindSRV(0);
    addShader->Unbind();
    //

    // Test if everything was computed correctly on the GPU
    Addition* result = (Addition*)outputBuffer->GetData();
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        if (result[i].value != addBuffer0[i].value + addBuffer1[i].value)
        {
            std::cout << "Compute shader failed" << std::endl;
            break;
        }
    }
    //

    // Cleanup
    delete buffer0;
    delete buffer1;
    delete outputBuffer;
    delete addShader;
}

void ImageFilter_Example()
{
    // Load the input image and create the output image
    CSH_ImageData* inputImage = new CSH_ImageData("images/forest.jpg");
    CSH_ImageData* outputImage = new CSH_ImageData(inputImage->Width, inputImage->Height);

    inputImage->BuildSRV();
    outputImage->BuildUAV();

    // Create the compute shader
    CSH_ComputeShader* filterShader = new CSH_ComputeShader(L"shaders/ImageFilter.hlsl");

    // Execute the compute shader
    filterShader->Bind();
    inputImage->BindSRV(0);
    outputImage->BindUAV(0);

    filterShader->Dispatch(inputImage->Width, inputImage->Height, 1);

    outputImage->UnbindUAV(0);
    inputImage->UnbindSRV(0);
    filterShader->Unbind();
    //

    // Write the output image to a file
    outputImage->OutputToImage("images/forest_sepia.png");

    // Cleanup
    delete inputImage;
    delete outputImage;
    delete filterShader;
}

int main()
{
    CSH_Init();

    ImageFilter_Example();
    
    CSH_Shutdown();
}