#define CL_TARGET_OPENCL_VERSION 210

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv){

    if(argc != 4){
        std::cerr << "input error\n";
        return -1;
    }
    int X = atoi(argv[1]);
    int Y = atoi(argv[2]);
    int Z = atoi(argv[3]);

    int *A = new int[X*Y]{0};
    int *B = new int[Y*Z]{0};
    int *ans = new int[X*Z]{0};
    int *gt = new int[X*Z]{0};

    for(int i = 0; i < X*Y; ++i)
        A[i] = i + 1;
    for(int i = 0; i < Y*Z; ++i)
        B[i] = i * 2;

    for(int i = 0; i < X; ++i){
        for(int j = 0; j < Z; ++j){
            for(int k = 0; k < Y; ++k ){
                gt[i*Z+j] += A[i*Y+k]*B[k*Z+j];
            }
        }
    }

    // Please parellelize the following code with OpenCL

	// Declare a memory object
    cl_mem MatA;
    cl_mem MatB;
    cl_mem MatAns;

    // Declare all we need
    cl_platform_id Platform;
    cl_device_id Device;
    cl_context Context;
    cl_command_queue commandQueue;
    cl_kernel Kernel;
    cl_program Program;

    // Store error code
    cl_int err;

    // Store available quantity
	cl_uint numPlatforms;
    cl_uint numDevices;

    //get the number of platform
    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    if(err != CL_SUCCESS){
        std::cerr<<"No platform found!\n";
        return -1;
    }
    //1.get and select an available plaform 
    err = clGetPlatformIDs(1, &Platform, NULL);

    int i, j;
    char* info;
    size_t infoSize;
    cl_uint platformCount;
    const char* attributeNames[5] = { "Name", "Vendor",
        "Version", "Profile", "Extensions" };
    const cl_platform_info attributeTypes[5] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR,
        CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };
    const int attributeCount = sizeof(attributeNames) / sizeof(char*);
    
    for (i = 0; i < platformCount; i++) {
            printf("\n %d. Platform \n", i+1);

            for (j = 0; j < attributeCount; j++) {

                // get platform attribute value size
                clGetPlatformInfo(Platform[i], attributeTypes[j], 0, NULL, &infoSize);
                info = (char*) malloc(infoSize);

                // get platform attribute value
                clGetPlatformInfo(Platform[i], attributeTypes[j], infoSize, info, NULL);

                printf("  %d.%d %-11s: %s\n", i+1, j+1, attributeNames[j], info);
                free(info);

            }    
        free(info);
    }

    // 2. get and select an available device (GPU) of the platform
    err = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, 1, &Device, &numDevices);
    if(err != CL_SUCCESS){
        std::cerr<<"No device found!\n";
        return -1;
    }

    for (i = 0; i < platformCount; i++) {

            // get all devices
            clGetDeviceIDs(Platform[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
            devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
            clGetDeviceIDs(Platform[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

            // for each device print critical attributes
            for (j = 0; j < deviceCount; j++) {

                // print device name
                clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
                printf("%d. Device: %s\n", j+1, value);
                free(value);

                // print hardware device version
                clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
                printf(" %d.%d Hardware version: %s\n", j+1, 1, value);
                free(value);

                // print software driver version
                clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
                printf(" %d.%d Software version: %s\n", j+1, 2, value);
                free(value);

                // print c version supported by compiler for device
                clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
                printf(" %d.%d OpenCL C version: %s\n", j+1, 3, value);
                free(value);

                // print parallel compute units
                clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                        sizeof(maxComputeUnits), &maxComputeUnits, NULL);
                printf(" %d.%d Parallel compute units: %d\n", j+1, 4, maxComputeUnits);

            }

            free(devices);
            free(Platform);
        }


    // 3. Create OpenCL context
    Context = clCreateContext(NULL, 1, &Device, NULL, NULL, &err);
    
	// 4. Creat command queue associate with the context
    commandQueue = clCreateCommandQueueWithProperties(Context, Device, NULL, &err);

	// 5. Create kernel program object for the file vecAdd.cl
    //Read kernel Code
    std::ifstream myfile;
    myfile.open("109006234_hw1.cl");
    if (!myfile.is_open()) {
            std::cerr << "Cannot open the file!\n";
            return -1;
    }

    std::string str((std::istreambuf_iterator<char>(myfile)), 
                     std::istreambuf_iterator<char>());
    myfile.close();
    
    //Create a Program objcet
    const char* strings = str.c_str();
    const size_t str_len = strlen(strings);
    Program = clCreateProgramWithSource(Context, 1, &strings, &str_len, &err);

    // 6. Build kernel program
    err = clBuildProgram(Program, 1, &Device, NULL, NULL, NULL);

    if (err != CL_SUCCESS) {
        size_t data_len = 0;
        clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, 0, NULL, &data_len);
        char *info = new char[data_len];
        clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, data_len, info, NULL);
        std::cerr << "Error building program!\n";
        delete[](info);
        return -1;
    }

    // 7. Create kernel object
    Kernel = clCreateKernel(Program, "matrixMul", &err);
    if(err != CL_SUCCESS){
        std::cerr<<"No kernel found!\n";
        return -1;
    }

	// 8. Create program input/output buffers
    MatA = clCreateBuffer(Context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(int) * (X*Y), (int*) malloc(sizeof(int) * (X*Y)), &err);
    MatB = clCreateBuffer(Context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(int) * (Y*Z), (int*) malloc(sizeof(int) * (Y*Z)), &err);
    MatAns = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(int) * (X*Z), NULL, &err);


	// 9. Set input/output buffers to the kernel's argument
    err = clSetKernelArg(Kernel, 0, sizeof(cl_mem), &MatA);
    err = clSetKernelArg(Kernel, 1, sizeof(cl_mem), &MatB);
    err = clSetKernelArg(Kernel, 2, sizeof(cl_mem), &MatAns);
    err = clSetKernelArg(Kernel, 3, sizeof(int), &X);
    err = clSetKernelArg(Kernel, 4, sizeof(int), &Y);
    err = clSetKernelArg(Kernel, 5, sizeof(int), &Z);

	// 10. Enqueue (run) the kernel
	size_t global_work_size[3] = {X,Y,Z};
	err = clEnqueueNDRangeKernel(commandQueue, Kernel, 3, NULL, global_work_size, NULL, 0, NULL, NULL);

	// 11. Read back the result from outputBuffer
	err = clEnqueueReadBuffer(commandQueue, MatAns, CL_TRUE, 0, (X*Z) * sizeof(int), ans, 0, NULL, NULL);

    for(int i = 0; i < X; ++i){
        for(int j = 0; j < Z; ++j){
            for(int k = 0; k < Y; ++k ){
                ans[i*Z+j] += A[i*Y+k]*B[k*Z+j];
            }
        }
    }

    // Check the answer
    for(int i = 0; i < X; ++i){
        for(int j = 0; j < Z; ++j){
            if(ans[i*Z+j] != gt[i*Z+j]){
                std::cerr<<"Failed.\n";
                return -1;
            }
        }
    }

    std::cout<<"AC\n";

    // Release the resources
    delete[] A;
    delete[] B;
    delete[] ans;
    delete[] gt;

    return 0;
}
