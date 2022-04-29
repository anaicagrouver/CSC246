// Elapsed Real Time for input-5.txt:
// GPU for this execution time:

// real    0m1.565s
// user    0m0.055s
// sys     0m1.208s

#include <stdio.h>
#include <stdbool.h>
#include <cuda_runtime.h>

// Input sequence of values.
int *vList;

// Number of values on the list.
int vCount = 0;

// Capacity of the list of values.
int vCap = 0;

// General function to report a failure and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Print out a usage message, then exit.
static void usage() {
  printf( "usage: zerosum [report]\n" );
  exit( 1 );
}

// Read the list of values.
__host__ void readList() {
  // Set up initial list and capacity.
  vCap = 5;
  vList = (int *) malloc( vCap * sizeof( int ) );

  // Keep reading as many values as we can.
  int v;
  while ( scanf( "%d", &v ) == 1 ) {
    // Grow the list if needed.
    if ( vCount >= vCap ) {
      vCap *= 2;
      vList = (int *) realloc( vList, vCap * sizeof( int ) );
    }

    // Store the latest value in the next array slot.
    vList[ vCount++ ] = v;
  }
}

__global__ void checkSum( int vCount, bool report,  int *gpuOut,  int *devList) {
  int idx = blockDim.x * blockIdx.x + threadIdx.x;
  //printf("idx %d vCount is %d \n", idx, vCount);
  if ( idx < vCount ) {
    //keeps track of sum
    int sum = 0; 
    //keeps track of count
    int count = 0; 
    for(int i = idx; i < vCount; i++){
      sum += devList[i];
      if(sum == 0){
        count++; 
        if(report)
          printf("%d .. %d\n",idx, i);
      }
    }
    gpuOut[idx] = count; 
}
}

int main( int argc, char *argv[] ) {
  if ( argc < 1 || argc > 2 )
    usage();

  // If there's an argument, it better be "report"
  bool report = false;
  if ( argc == 2 ) {
    if ( strcmp( argv[ 1 ], "report" ) != 0 )
      usage();
    report = true;
  }

  readList();

  // Allocate space on the device to hold a copy of the sequence.
  int *devList = NULL;
  if ( cudaMalloc((void **)&devList, vCount * sizeof(int) ) != cudaSuccess )
    fail( "Failed to allocate space for lenght list on device" );
  // Copy the sequence over to the device.
  if ( cudaMemcpy( devList, vList, vCount * sizeof(int),
                    cudaMemcpyHostToDevice) != cudaSuccess )
      fail( "Failed to copy list to device" );
  // Maybe some more code written by you.
  int *gpuOut = NULL;
  cudaMalloc( (void **)&gpuOut, vCount * sizeof(int));
  // Block and grid dimensions.
  int threadsPerBlock = 100;
  // Round up for the number of blocks we need.
  int blocksPerGrid = ( vCount + threadsPerBlock - 1 ) / threadsPerBlock;

  // Run our kernel on these block/grid dimensions
  checkSum<<<blocksPerGrid, threadsPerBlock>>>( vCount, report, gpuOut, devList );
  if ( cudaGetLastError() != cudaSuccess )
    fail( "Failure in CUDA kernel execution." );

  // Add code to copy results back to the host and then add up the total
  // number of zero-sum ranges found.
  int result[vCount];
  cudaMemcpy(result, gpuOut, vCount * sizeof(int), cudaMemcpyDeviceToHost);

  //calculating total 
  int total = 0;
  for(int i = 0; i < vCount; i++)
    total += result[i];
  printf( "Total: %d\n", total );

  // Free memory on the device and the host.
  cudaFree( devList );
  free( vList );
  cudaFree( gpuOut );
  cudaDeviceReset();
  return 0;
}
