# Image segmentation via region growing in C++ and OpenCV

### Introduction
This code provides a segmentation of the given image, following the region growing algorithms. A first segmentation is done. Afterwards, the program joins the adjacent similar regions. A first fast computation of the initial regions' boundaries is also done. If wanted, a more time expensive computation of the final merged regions can also be done.

### Outcome
Given this as an initial image.

One initial segmentation is:

When merged, the outcome is:

The fast initial boundaries computation is:

It required ``2.27`` seconds in total in a MacBook Pro 2017.

Given the same initial image, with the purpose of computing the actual final boundaries, the outcomes are:



The computation of the merged regions' boundaries required ``197``seconds.

Different segmentation criteria can be given. For instance, it can be the light intensity, leading to the following segmentation:

