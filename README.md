# Neuron Image Denoise
Preprocessing the neuron image signals to make it benign for reconstruction.



## Download & Compile

As the `3rdparty` folder contains submodule references. use `--recurse-submodule` for your clone command.

```bash
git clone --recurse-submodule https://github.com/SEU-ALLEN-codebase/plugin-neuron-image-denoise.git
```

By changing some variables in the `src/neuron-image-denoise.pro`, you can make it like a typical Vaa3D plugin.

Note, the code is based on Qt6 and the build may only be used with Vaa3D based on Qt6.



## Usage Guide

We offer an integrated function `integrated_filter` to denoise typical neuron images, with already tuned parameters for fMOST images. That being said, you can drop some of the steps by calling each function separately. Each step contains their own parameters, and you can also tune them in the integrated function..

Basically, the first step, `denoise` is all you need. It uses adaptive threshold to denoise background and use a similar mechanism to denoise flare caused by the imaging technique. What makes it tricky is that we found different versions of fMOST can introduce very diverse artifacts, so the filter is designed flexible and tuned versatile, but for a specific kind of image, a better parameter setting may exist. e.g., in the newest HD-fMOST, the flare effect is maximally reduced, so you can turn that down.

The other steps are for clearing up other problems. The adaptive thresholding is essentially a high pass filter, meaning high frequency noises can remain, and Gaussian blurring is for that. For images of drastic quality, after these steps, fibers can be broken, causing troubles for some reconstruction algorithms. With downsampling, the gaps can be filled and the image may look more isotropic.

The background of the filtered image is largely 0 (as what the adaptive filter usually does), so you can trace with a threshold of 0 in most cases. In very rare cases, where glia and brain region borders can be left, an over trace may happen.

A typical problem caused by this filter is that the soma becomes more hollow and its flare removed so that your soma annotation may be off the signal and your reconstruction may fail. It's advised to always to a soma refinement again. There's a mean shift plugin in Vaa3D for that.

PLEASE INPUT 16BIT RAW IMAGES FOR BETTER RESULTS. This denoising plugin is aimed at extracting the high frequency textures that can be neuron fibers, but images converted down to 8bit have already lost a lot of them. Also, enhancing techniques preceding this can lead to unpredictable results, since some artifacts can also be enhanced and exceed the capability of this plugin.



### UI

The function of denoising, blurring, max downsampling and the integrated filter are provided and their panels share the parameters with CLI.



### CLI

* **denoise**: remove background and flare effect.
* **blur**: remove steep background noise overlooked by denoising.
* **max_ds**: downsample to merge the gap between signals induced by previous steps. Optional for reconstruction algorithms that allow small gaps.
* **integrated_filter**: execute the above procedures sequentially, i.e. denoise -> blur -> max_ds
* **guo_enh**: Guo's image enhancement.
* **search_soma**: search soma by meanshift and finding maxima in GSDT.



CLI functions can be parameterized through `-p`, but different from other Vaa3D plugins, in that you can specify them as `<Name> <Value>` pairs.

```bash
# basic denoise
v3d -x neuron_image_denoise -f denoise -i in.tif -o out.tif -p \
	ada_interval 3,3,2 ada_sampling 3 flare_interval 8,8,2 flare_sampling 8 \
	flare_weight 0.02 atten_depth 4 flare_x true flare_y true;

# gaussian blur
v3d -x neuron_image_denoise -f blur -i in.tif -o out.tif -p blur_sigma 1,1,1

# downsampling
v3d -x neuron_image_denoise -f max_ds -i in.tif -o out.tif -p scale 4,4,1

# integrated
vaa3d -x neuron_image_rectify -f integrated_filter -i in.tif -o out.tif out.marker -p [parameters related]

# Guo's image inhancement
v3d -x neuron_image_denoise -f guo_enh -i in.tif -o out.tif -p \
	enh_sigma 1,1,0.25 color_sigma 35 gain 5 cutoff 25 bilateral true fft true

# soma search
v3d -x neuron_image_denoise -f search_soma -i in.tif in.marker -o out.marker -p 
	soma_radius 4,4,4 sigma 5,5,5 bg_thr 0 z_thickness 1 cnn_type 3 test_count 10
```

