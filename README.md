# matconv


The goal is to optimize the convolution step of a neural network.

The input data is a set of 2D images (w x h) that has a depth (d) (or number of channels), that is to be convolved with a set of filters (N_F) that has size (wf x hf) and a matching depth. During convolution, we sum over the depth, but keep the dimension of the set of filters, so the output is a set (N_F elements) of 2D images with width: ((w - wf + 1) x (h - hf + 1)).

A reference CPU version is provided, the code only has to work for the specific sizes provided in the reference implementation and the reference input/output files: http://u235axe.web.elte.hu/GPUCourse/Convolution/

In the GPU implementation, use the fast Winograd method (https://arxiv.org/pdf/1509.09308.pdf), utilize shared memory and textures to accelerate the computation as much as possible. Always check that the computation is valid on the provided reference output file. The maximum element wise error should be smaller than 3e-5f.

A single correct implementation gives 40%, a series of implementations showing how different optimizations improved (or not improved) performance with a small documentation of the ideas behind the different approaches can give an additional +20%.


