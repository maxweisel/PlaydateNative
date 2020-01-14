//
//  main.m
//  ImageProcessor
//
//  Created by Max Weisel on 1/13/20.
//  Copyright © 2020 Max Weisel. All rights reserved.
//

#import <Foundation/Foundation.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSMutableIndexSet *colors = [[NSMutableIndexSet alloc] init];
        
        NSMutableArray *bitmap = [[NSMutableArray alloc] initWithCapacity:400*240];
        
        NSURL *fileURL = [NSURL fileURLWithPath:@"/Users/maxweisel/Dropbox/Projects/52 Playdate Experiments/3 PlaydateNative/SuperMarioBros.png"];
        CGImageSourceRef imageSource = CGImageSourceCreateWithURL(CFBridgingRetain(fileURL), NULL);
        CGImageRef image = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
        NSLog(@"Width: %i", (int)CGImageGetWidth(image));
        CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(image));
        uint8_t *imageBytes = (uint8_t *)CFDataGetBytePtr(imageData);
        for (int i = 0; i < 400*240; i++) {
            uint8_t r = imageBytes[i*4];
            uint8_t g = imageBytes[i*4+1];
            uint8_t b = imageBytes[i*4+2];
            //uint8_t a = imageBytes[i*4+3];
            [colors addIndex:r];
            [colors addIndex:g];
            [colors addIndex:b];
            //if (r != 255 && g != 255 && b != 255 && r != 0 && g != 0 && b != 0)
            //    printf("(%i, %i, %i, %i) ", r, g, b, a);
            
            int colorIndex = 0;
                 if (r < 2) colorIndex  = 0;
            else if (r < 66) colorIndex = 1;
            else if (r < 129) colorIndex = 2;
            else if (r < 194) colorIndex = 3;
            else              colorIndex = 4;
            [bitmap addObject:[NSNumber numberWithInt:colorIndex]];
        }
        
        NSMutableString *bitmapString = [[NSMutableString alloc] initWithString:@"int bitmap[] = { "];
        for (NSNumber *pixel in bitmap) {
            [bitmapString appendFormat:@"%i, ", pixel.intValue];
        }
        [bitmapString appendString:@"};"];
        
        printf("%s\n", [bitmapString UTF8String]);
    }
    return 0;
}
