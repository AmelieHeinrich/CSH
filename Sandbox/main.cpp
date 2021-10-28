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

    CSH_ComputeShader* addShader = new CSH_ComputeShader(L"shaders/Add.hlsl");

    CSH_Buffer* buffer0 = new CSH_Buffer(sizeof(Addition), NUM_ELEMENTS, &addBuffer0[0]);
    CSH_Buffer* buffer1 = new CSH_Buffer(sizeof(Addition), NUM_ELEMENTS, &addBuffer1[0]);
    CSH_Buffer* outputBuffer = new CSH_Buffer(sizeof(Addition), 1024, nullptr);

    buffer0->BuildSRV();
    buffer1->BuildSRV();
    outputBuffer->BuildUAV();

    addShader->Bind();
    buffer0->BindSRV(0);
    buffer1->BindSRV(1);
    outputBuffer->BindUAV(0);

    addShader->Dispatch(NUM_ELEMENTS, 1, 1);

    outputBuffer->UnbindUAV(0);
    buffer1->UnbindSRV(1);
    buffer0->UnbindSRV(0);
    addShader->Unbind();

    Addition* result = (Addition*)outputBuffer->GetData();
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        if (result[i].value != addBuffer0[i].value + addBuffer1[i].value)
        {
            std::cout << "Compute shader failed" << std::endl;
            break;
        }
    }

    delete buffer0;
    delete buffer1;
    delete outputBuffer;
    delete addShader;
}

int main()
{
    CSH_Init();

    Addition_Example();
    
    CSH_Shutdown();
}