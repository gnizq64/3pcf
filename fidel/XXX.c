#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <omp.h>
#include <time.h>

float d ( float x[4], float y[4] )   // function definition
{
    float p ;
    p = sqrt( pow( x[0]-y[0], 2) + pow( x[1]-y[1], 2) + pow( x[2]-y[2], 2) ) ;
    return ( p ) ;
}

int main( int argc, char * argv [] ) {
    
    int i,j,a,b,n,k,c;
    int n_bins=18;
    int hist[n_bins+1][n_bins+1][n_bins+1];
    float x, y, z, w;
    float r1,r2,r3;
    float L=1000;
    int n_part=10;
    float l=L/n_part;
    int n_box=n_part*n_part*n_part;
    
    char name_out[80];
    strcpy(name_out,argv[2]);
    strcat(name_out,".dat");
    
    char data_in[80];
    strcpy(data_in,argv[1]);
    
    float d_max=50.0;
    float **data_point;
    data_point=(float **)malloc(10000000*sizeof(float *));
    float ***G;
    G=(float ***)malloc(10000000*sizeof(float **));
    
    for (i =0; i <10000000; i++){
        data_point[i]=(float *)malloc(4*sizeof(float));
    }
    
    
    FILE *data, *out;
    data=fopen(data_in,"r");
    while( !feof(data) ){
        fscanf(data, "%f %f %f %f", &x, &y, &z ,&w);
        data_point[n][0] = x;
        data_point[n][1] = y;
        data_point[n][2] = z;
        data_point[n][3] = w;
        n++;
    }
    fclose(data);
    
    for(i = 0; i < n_bins; i++){
        for(j = 0; j < n_bins; j++){
            for(k = 0; k < n_bins; k++){
                hist[i][j][k]=0;
            }
        }
    }
    
    
    int C[n_box];
    int C_count[n_box];
    for(i = 0; i<n_box; i++){
        C[i]=0;
        C_count[i]=0;
    }
    
    for(i = 0; i < n; i++){
        x=data_point[i][0];
        y=data_point[i][1];
        z=data_point[i][2];
        k=n_part*n_part*(int)(z/l)+n_part*(int)(y/l)+(int)(x/l);
        C[k]+=1;
    }
    
    int m=C[0];
    for(i=0;i<n_box;i++){
        if (m<C[i])
            m=C[i];
        
    }
    
    
    for(i = 0; i< n_box; i++) {
        G[i] = (float **) malloc((m+1)*sizeof(float *));
        for(j = 0; j < m+1; j++) {
            G[i][j] = (float *)malloc(4*sizeof(float));
        }
    }
    
    
    for( i= 0; i < n_box; i++){
        for(j = 0; j < m+1; j++){
            for( k = 0;k < 3; k++){
                G[i][j][k]=0;
            }
        }
    }
    
    
    for(i=0;i<n;i++){
        x=data_point[i][0];
        y=data_point[i][1];
        z=data_point[i][2];
        k=n_part*n_part*(int)(z/l)+n_part*(int)(y/l)+(int)(x/l);
        G[k][C_count[k]][0]=x;
        G[k][C_count[k]][1]=y;
        G[k][C_count[k]][2]=z;
        C_count[k]+=1;
    }
    
    for(i=0;i<n_part;i++){
        for(j=0;j<n_part;j++){
            for(k=0;k<n_part;k++){
                //printf("caja %i\n ", k+j*n_part+i*n_part*n_part+1);
                G[k+j*n_part+i*n_part*n_part][m][0]=k*l+l/2;
                G[k+j*n_part+i*n_part*n_part][m][1]=j*l+l/2;
                G[k+j*n_part+i*n_part*n_part][m][2]=i*l+l/2;
            }
        }
    }
    
    float aux0=sqrt(3)*l;
    float d_box,d_box1,d_box2;
#pragma omp parallel private(i,j,k,r1,r2,r3,d_box,d_box1,d_box2,a,b,c)
    {
        int S[19][19][19] = {0};
#pragma omp for schedule(dynamic) private(i,j,k,r1,r2,r3,d_box,d_box1,d_box2,a,b,c)
        for(i=0;i<n_box;i++){
            printf("%i\n",i);
            for(a=0;a<C[i];a++){
                for(b=a+1;b<C[i];b++){
                    r1=d(G[i][a],G[i][b]);
                    if (r1 < d_max){
                        for(c=b+1;c<C[i];c++){
                            r2=d(G[i][a],G[i][c]);
                            r3=d(G[i][b],G[i][c]);
                            if (r2<d_max && r3<d_max && r1<r2+r3 && r3<r2+r1 && r2<r3+r1){
                                S[(int)(r1/(d_max/n_bins))][(int)(r2/(d_max/n_bins))][(int)(r3/(d_max/n_bins))]+=1;
                                S[(int)(r2/(d_max/n_bins))][(int)(r3/(d_max/n_bins))][(int)(r1/(d_max/n_bins))]+=1;
                                S[(int)(r3/(d_max/n_bins))][(int)(r1/(d_max/n_bins))][(int)(r2/(d_max/n_bins))]+=1;
                            }
                        }
                    }
                }
            }
            for(j=i+1;j<n_box;j++){
                d_box=d(G[i][m],G[j][m]);
                if (d_box-aux0<d_max){
                    for(a=0;a<C[i];a++){
                        for(b=0;b<C[j];b++){
                            r1=d(G[i][a],G[j][b]);
                            if (r1 < d_max){
                                for(c=b+1;c<C[j];c++){
                                    r2=d(G[i][a],G[j][c]);
                                    r3=d(G[j][b],G[j][c]);
                                    if (r2<d_max && r3<d_max && r1<r2+r3 && r3<r2+r1 && r2<r3+r1){
                                        S[(int)(r1/(d_max/n_bins))][(int)(r2/(d_max/n_bins))][(int)(r3/(d_max/n_bins))]+=1;
                                        S[(int)(r2/(d_max/n_bins))][(int)(r3/(d_max/n_bins))][(int)(r1/(d_max/n_bins))]+=1;
                                        S[(int)(r3/(d_max/n_bins))][(int)(r1/(d_max/n_bins))][(int)(r2/(d_max/n_bins))]+=1;
                                    }
                                }
                                for(c=a+1;c<C[i];c++){
                                    r2=d(G[i][a],G[i][c]);
                                    r3=d(G[j][b],G[i][c]);
                                    if (r2<d_max && r3<d_max && r1<r2+r3 && r3<r2+r1 && r2<r3+r1){
                                        S[(int)(r1/(d_max/n_bins))][(int)(r2/(d_max/n_bins))][(int)(r3/(d_max/n_bins))]+=1;
                                        S[(int)(r2/(d_max/n_bins))][(int)(r3/(d_max/n_bins))][(int)(r1/(d_max/n_bins))]+=1;
                                        S[(int)(r3/(d_max/n_bins))][(int)(r1/(d_max/n_bins))][(int)(r2/(d_max/n_bins))]+=1;
                                    }
                                }
                                for(k=j+1;k<n_box;k++){
                                    d_box1=d(G[i][m],G[k][m]);
                                    d_box2=d(G[j][m],G[k][m]);
                                    if( d_box1-aux0<d_max && d_box2-aux0<d_max ){
                                        for(c=0;c<C[k];c++){
                                            r2=d(G[i][a],G[k][c]);
                                            r3=d(G[j][b],G[k][c]);
                                            if (r2<d_max && r3<d_max && r1<r2+r3 && r3<r2+r1 && r2<r3+r1){
                                                S[(int)(r1/(d_max/n_bins))][(int)(r2/(d_max/n_bins))][(int)(r3/(d_max/n_bins))]+=1;
                                                S[(int)(r2/(d_max/n_bins))][(int)(r3/(d_max/n_bins))][(int)(r1/(d_max/n_bins))]+=1;
                                                S[(int)(r3/(d_max/n_bins))][(int)(r1/(d_max/n_bins))][(int)(r2/(d_max/n_bins))]+=1;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
#pragma omp critical
        {
            for( a = 0; a < n_bins; a++) {
                for( b = 0; b < n_bins; b++) {
                    for( c = 0; c < n_bins; c++) {
                        hist[a][b][c]+=S[a][b][c];
                    }
                }
            }
        }
    }
    
    out=fopen(name_out,"w");
    int ii;
    for( ii = 0; ii < n_bins; ii++){
        for( j = 0; j < n_bins; j++){
            for( k = 0; k < n_bins; k++){
                fprintf(out,"%i ", hist[ii][j][k]);
            }
            fprintf(out,"  \n ");
        }
        fprintf(out,"  \n ");
    }
    
    fclose(out);
    
    return(0);
}
