#include "mylib.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void matrixSquare( float * mat, size_t size, float ** res ) {
    for( int j=0; j<size; ++j ) {
        for( int i=0; i<size; ++i ) {
            float val = 0.0f;
            for( int k=0; k<size; ++k ) {
                 val += mat[i + size*k] * mat[k + size*j];
            }
            (*res)[i + size*j] = val;
        }
    }
}

void TEST_matrixSquare( ) {
    size_t const N = 3;
    float * mat = new float[ N*N ];
    float * res = new float[ N*N ];

    memset( mat, 0, sizeof( float ) * N*N );
    memset( res, 0, sizeof( float ) * N*N );

    mat[0] = 2; mat[1] = 0; mat[2] = 0;
    mat[3] = 0; mat[4] = 3; mat[5] = 0;
    mat[6] = 0; mat[7] = 0; mat[8] = 5;

    for( int i=0; i<N; ++i ) {
        for( int j=0; j<N; ++j ) {
            printf( " %5g ", mat[j+N*i] );
        }
        puts( " " );
    }
    puts( " " );

    matrixSquare( mat, N, &res );

    for( int i=0; i<N; ++i ) {
        for( int j=0; j<N; ++j ) {
            printf( " %5g ", res[j+N*i] );
        }
        puts( " " );
    }

    delete[] mat;
    delete[] res;
}

void TEST_bubbleSort( ) {
    int const N = 512;
    float * arr = new float[N];

    for( int i=0; i<N; ++i ) {
        arr[i] = N-i;
    }

    int j=0;
    for( int i=0; i<N; ++i ) {
        printf( "%4g", arr[i] );
        ++j;
        if( j == 80/4 -1 ) {
            puts( " " );
            j=0;
        }
    }
    puts( " " );

    bubbleSort( arr, N );

    /*int*/ j=0;
    for( int i=0; i<N; ++i ) {
        printf( "%4g", arr[i] );
        ++j;
        if( j == 80/4 -1 ) {
            puts( " " );
            j=0;
        }
    }
    puts( " " );
}

void TEST_alternatingHarmoincSeries( ) {
    size_t const N = 8000000;
    double res;
    alternatingHarmonicSeries( N, res );
    printf( "res: %12g ln 2: 0.69314718055994530941723212145818", res );
}

void TEST_logMcLoren( ) {

   double eks = 0.01; 
   double res = 0.0f;

   logMcLoren( eks, 20, res );

   printf( "mcLoren res: %g\nlib log res: %g\n", res, log( eks + 1 ) );
}

void TEST_logOverArray() {
    srand( time(NULL) );
    size_t const N = 5;
    double * arr = new double[N];
    for( int i=0; i<N; ++i ) {
        arr[i] = rand() / RAND_MAX;
    }
    logOverArray( arr, N );
}

