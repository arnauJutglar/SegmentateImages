# Image segmentation via region growing in C++ and OpenCV

### Introduction
This code provides a segmentation of the given image, following the region growing algorithms. A first segmentation is done. Afterwards, the program joins the adjacent similar regions. A first fast computation of the initial regions' boundaries is also done. If wanted, a more time expensive computation of the final merged regions can also be done.

### Outcome
Given this as an initial image:

![initial](https://github.com/arnauJutglar/SegmentateImages/blob/main/resources/Original.png)

One initial segmentation is:

![segmentation1](https://github.com/arnauJutglar/SegmentateImages/blob/main/resources/segmentated1.png)

When merged, the outcome is:

![merged1](https://github.com/arnauJutglar/SegmentateImages/blob/main/resources/merged1.png)

The fast initial boundaries computation is:

![boundaries1](https://github.com/arnauJutglar/SegmentateImages/blob/main/resources/boundaries1.png)

It required ``2.27`` seconds in total in a MacBook Pro 2017.

Given the same initial image, with the purpose of computing the actual final boundaries, the outcomes are:

![segmentated3](https://github.com/arnauJutglar/SegmentateImages/blob/main/resources/segmentated3.png)
![merged3](https://github.com/arnauJutglar/SegmentateImages/blob/main/resources/merged3.png)
![boundaries3](https://github.com/arnauJutglar/SegmentateImages/blob/main/resources/boundaries3.png)


The computation of the merged regions' boundaries required ``197``seconds.

Different segmentation criteria can be given. For instance, it can be the light intensity, leading to the following segmentation:

![segmentation4](https://github.com/arnauJutglar/SegmentateImages/blob/main/resources/merged4.png)

### Usage
All the code is factorized inside the Segmentator class. Any instance of it must be initialized with the image that has to be segmentated. If wanted, a similarity function can be set via the ``setSimilarityFunction()`` method. Otherwise, it uses the default one, which is given by the average color similarity. The number of initial seeds can also be changed via the ``setNumSeeds()`` method (default = ``20``).

The outcome images can be obtained via the methods ``getLabels()``, ``getUnmergedLabels()`` and ``getBoundary()``.

For the segmentation to be made, a call to the ``segmentate()`` method must be done. If it is wanted to compute the merged boundaries, a call to ``computeBoundary()`` must also be done.

### Dependencies
Dependencies to the OpenCV source code are given in this repository. The provided Makefile is able to compilate the program without any other requirement than to download the whole repository. However, this OpenCV version has been made for MacOS so there's no warranty that it will work in a different OS.