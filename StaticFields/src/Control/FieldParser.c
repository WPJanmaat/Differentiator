#include "FieldParser.h"

Field ParseField(char *filepath, FieldProperties properties) {
    //expected input is the CSV generated by the python script. 

    FILE* file = fopen(filepath, "r");

    Field output;
    long double Xstep = properties.Xstep;
    long double Ystep = properties.Ystep;
    long double Zstep = properties.Zstep;

    //preliminary readings
    output.startX = properties.XStart;
    output.steplengthX = Xstep;

    output.startY = properties.YStart;
    output.steplengthY = Ystep;
    
    output.startZ = properties.ZStart;
    output.steplengthZ = Zstep;

    //this could be updated to include 2D fields.
    if(Xstep == 0 || Ystep == 0 || Zstep == 0) {
        fprintf(stderr, "0 steplength for X, Y or Z is not allowed in this program, please use a valid 3D field\n");
        EXIT_FAILURE;
    }

    //distrust of floating point inaccuracies.
    int xsize = (int)(((properties.Xend-properties.XStart)/Xstep)+1);
    int ysize = (int)(((properties.Yend-properties.YStart)/Ystep)+1);
    int zsize =(int)(((properties.Zend-properties.ZStart)/Zstep)+1);

    //initialise arrays based on (hopefully good) size estimates.
    //the floating poitns here are a headache
    output.FieldValues = calloc(sizeof(Vector**), xsize);
    for (int i = 0; i< xsize; i++) {
        output.FieldValues[i] = calloc(sizeof(Vector*), ysize);
        for (int j = 0; j< ysize; j++) {
            output.FieldValues[i][j] = calloc(sizeof(Vector), zsize);
        }
    }

    int check; //for EOF check
    //preliminary declarations for the loop
    long double currentZ;
    long double currentY;
    long double currentX;
    long double newZ = properties.ZStart;
    long double newY = properties.YStart;
    long double newX = properties.XStart;
    int xindex = 0;
    int yindex = 0;
    int zindex = 0;

    output.lengthX = xsize-1;
    output.lengthY = ysize-1;
    output.lengthZ = zsize-1;

    //the function is quite verbose: each read is accompanied with an EOF check for ungraceful exits, no error handling is implemented, imperfect reads are carried on.
    do {
        do {
            do {
                Vector newVec;
                currentX = newX; 
                currentY = newY; 
                currentZ = newZ; 
                check = fscanf(file, "%Lf,", &newX); // read X position

                if (check == 0) fprintf(stderr, "Warning, failed to read new X position at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                //if there is a graceful break, it is here.
                if (check == EOF) break;

                check = fscanf(file, "%Lf,", &newY); // read Y position
                if (check == 0) fprintf(stderr, "Warning, failed to read new Y position at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                if (check == EOF) {
                    fprintf(stderr, "Warning, ungraceful break while reading new Y position at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                    break;
                }

                check = fscanf(file, "%Lf,", &newZ); // read Z position
                if (check == 0) fprintf(stderr, "Warning, failed to read new Z position at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                if (check == EOF) {
                    fprintf(stderr, "Warning, ungraceful break while reading new Z position at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                    break;
                }

                check = fscanf(file, "%Lf,", &newVec.x); // read X value
                if (check == 0) fprintf(stderr, "Warning, failed to read new X value at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                if (check == EOF) {
                    fprintf(stderr, "Warning, ungraceful break while reading new X value at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                    break;
                }

                check = fscanf(file, "%Lf,", &newVec.y); // read Y value
                if (check == 0) fprintf(stderr, "Warning, failed to read new Y value at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                if (check == EOF) {
                    fprintf(stderr, "Warning, ungraceful break while reading new Y value at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                    break;
                }

                check = fscanf(file, "%Lf\n", &newVec.z); // read Z value
                if (check == 0) fprintf(stderr, "Warning, failed to read new Z value at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                if (check == EOF) {
                    fprintf(stderr, "Warning, ungraceful break while reading new Z value at X: %d, Y: %d, Z: %d \n", xindex, yindex, zindex);
                    break;
                }

                output.FieldValues[xindex][yindex][zindex] = newVec;
                xindex++;

                //size checks
                //this *should* only happen once at most, but floating points are not to be trusted.
                if(xindex > xsize) {
                    xsize += 100;
                    realloc(output.FieldValues, sizeof(Vector**)*xsize);
                }
                if(output.lengthX <= xindex) {
                    output.lengthX = xindex+1;
                }

            } while ((Xstep < 0 && newX < (currentX - (Xstep/2))) ||(Xstep > 0 && newX > (currentX - (Xstep/2))));
            
            //for exit.
            if(check == EOF) break;
            yindex++;
            //size checks
            if(yindex > ysize) {
                ysize += 100;
                realloc(output.FieldValues[xindex], sizeof(Vector*)*xsize);
            }
            if(output.lengthY <= yindex) {
                output.lengthY = yindex+1;
            }

        } while ((Ystep < 0 && newY < (currentY - (Ystep/2))) ||(Ystep > 0 && newY > (currentY - (Ystep/2))));
    
    if(zindex>zsize) {
        zsize +=100;
        realloc(output.FieldValues[xindex][yindex], sizeof(Vector)*zsize);
    }
    if(output.lengthZ <= yindex) {
        output.lengthZ = zindex +1;
    }

    } while (check != EOF);
    return output;
}