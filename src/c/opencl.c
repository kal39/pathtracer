#include "opencl.h"

CLProgram create_cl_program(char *fileName, char *kernelName) {
	CLProgram clProgram;

	// get platforms
	cl_uint platformNum;
	clGetPlatformIDs(1, &clProgram.platform, &platformNum);

	// get devices
	cl_uint deviceNum;
	clGetDeviceIDs(clProgram.platform, CL_DEVICE_TYPE_GPU, 2, &clProgram.device, &deviceNum);

	// create context and command queue
	clProgram.context = clCreateContext(0, 1, &clProgram.device, NULL, NULL, NULL);
	clProgram.queue = clCreateCommandQueueWithProperties(clProgram.context, clProgram.device, 0, NULL);

	// load program
	const char *source = read_file(fileName);
	clProgram.program = clCreateProgramWithSource(clProgram.context, 1, &source, NULL, NULL);
	free((char *)source);

	// build program
	if(clBuildProgram(clProgram.program, 0, NULL, "-I src/cl -Werror -cl-mad-enable -cl-no-signed-zeros -cl-fast-relaxed-math", NULL, NULL) != CL_SUCCESS) {
		msg("Failed to build clProgram.program\n");

		char buffer[0x100000];
		clGetProgramBuildInfo(clProgram.program, clProgram.device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
		msg("%s\n", buffer);
		exit(1);
	}

	// create kernel object
	clProgram.kernel = clCreateKernel(clProgram.program, kernelName, NULL);

	return clProgram;
}

int run_cl_program(CLProgram clProgram, size_t workItems, int verbose) {
	cl_int ret = clEnqueueNDRangeKernel(clProgram.queue, clProgram.kernel, 1, NULL, &workItems, NULL, 0, NULL, NULL);

	if(ret != CL_SUCCESS)
		return 0;
		
	if(verbose) {
		switch(ret) {
			case CL_INVALID_PROGRAM_EXECUTABLE:
				printf("there is no successfully built program executable available for device associated with command_queue.\n");
				break;
			case CL_INVALID_COMMAND_QUEUE:
				printf("command_queue is not a valid host command-queue.\n");
				break;
			case CL_INVALID_KERNEL:
				printf("kernel is not a valid kernel object.\n");
				break;
			case CL_INVALID_CONTEXT:
				printf("context associated with command_queue and kernel are not the same or if the context associated with command_queue and events in event_wait_list are not the same.\n");
				break;
			case CL_INVALID_KERNEL_ARGS:
				printf("the kernel argument values have not been specified or if a kernel argument declared to be a pointer to a type does not point to a named address space.\n");
				break;
			case CL_INVALID_WORK_DIMENSION:
				printf("work_dim is not a valid value (i.e. a value between 1 and CL_​DEVICE_​MAX_​WORK_​ITEM_​DIMENSIONS).\n");
				break;
			case CL_INVALID_GLOBAL_WORK_SIZE:
				printf("any of the values specified in global_work_size[0], …​ global_work_size[work_dim - 1] exceed the maximum value representable by size_t on the device on which the kernel-instance will be enqueued.\n");
				printf("any of the values specified in global_work_size[0], …​ global_work_size[work_dim - 1] exceed the maximum value representable by size_t on the device on which the kernel-instance will be enqueued.\n");
				break;
			case CL_INVALID_GLOBAL_OFFSET:
				printf("the value specified in global_work_size + the corresponding values in global_work_offset for any dimensions is greater than the maximum value representable by size t on the device on which the kernel-instance will be enqueued.\n");
				break;
			case CL_INVALID_WORK_GROUP_SIZE:
				printf("local_work_size is specified and does not match the required work-group size for kernel in the program source.\n");
				printf("local_work_size is specified and is not consistent with the required number of sub-groups for kernel in the program source.\n");
				printf("local_work_size is specified and the total number of work-items in the work-group computed as local_work_size[0] × …​ local_work_size[work_dim - 1] is greater than the value specified by CL_​KERNEL_​WORK_​GROUP_​SIZE in the Kernel Object Device Queries table.\n");
				printf("the program was compiled with cl-uniform-work-group-size and the number of work-items specified by global_work_size is not evenly divisible by size of work-group given by local_work_size or by the required work-group size specified in the kernel source.\n");
				printf("the number of work-items specified in any of local_work_size[0], …​ local_work_size[work_dim - 1] is greater than the corresponding values specified by CL_​DEVICE_​MAX_​WORK_​ITEM_​SIZES[0], …​, CL_​DEVICE_​MAX_​WORK_​ITEM_​SIZES[work_dim - 1].\n");
				break;
			case CL_MISALIGNED_SUB_BUFFER_OFFSET:
				printf("a sub-buffer object is specified as the value for an argument that is a buffer object and the offset specified when the sub-buffer object is created is not aligned to CL_​DEVICE_​MEM_​BASE_​ADDR_​ALIGN value for device associated with queue. This error code is missing before version 1.1.\n");
				break;
			case CL_INVALID_IMAGE_SIZE:
				printf("an image object is specified as an argument value and the image dimensions (image width, height, specified or compute row and/or slice pitch) are not supported by device associated with queue.\n");
				break;
			case CL_IMAGE_FORMAT_NOT_SUPPORTED:
				printf("an image object is specified as an argument value and the image format (image channel order and data type) is not supported by device associated with queue.\n");
				break;
			case CL_OUT_OF_RESOURCES:
				printf("there is a failure to queue the execution instance of kernel on the command-queue because of insufficient resources needed to execute the kernel. For example, the explicitly specified local_work_size causes a failure to execute the kernel because of insufficient resources such as registers or local memory. Another example would be the number of read-only image args used in kernel exceed the CL_​DEVICE_​MAX_​READ_​IMAGE_​ARGS value for device or the number of write-only and read-write image args used in kernel exceed the CL_​DEVICE_​MAX_​READ_​WRITE_​IMAGE_​ARGS value for device or the number of samplers used in kernel exceed CL_​DEVICE_​MAX_​SAMPLERS for device.\n");
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("there is a failure to allocate memory for data store associated with image or buffer objects specified as arguments to kernel.\n");
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events.\n");
				break;
			case CL_INVALID_OPERATION:
				printf("SVM pointers are passed as arguments to a kernel and the device does not support SVM or if system pointers are passed as arguments to a kernel and/or stored inside SVM allocations passed as kernel arguments and the device does not support fine grain system SVM allocations.\n");
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("there is a failure to allocate resources required by the OpenCL implementation on the host.\n");
				break;
			default:
				break;
		}
	}

	clFinish(clProgram.queue);

	return 1;
}

void destroy_cl_program(CLProgram clProgram) {
	clReleaseProgram(clProgram.program);
	clReleaseKernel(clProgram.kernel);
	clReleaseCommandQueue(clProgram.queue);
	clReleaseContext(clProgram.context);
}