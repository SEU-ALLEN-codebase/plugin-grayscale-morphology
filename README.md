# Neuron Image Denoise
Preprocessing the neuron image signals to make it benign for reconstruction.

## Commands

* **denoise**: remove background and flare effect.
* **blur**: remove steep background noise overlooked by denoising.
* **max_ds**: downsample to merge the gap between signals induced by previous steps. Optional for reconstruction algorithms that allow small gaps.
* **auto_thr**: estimate a normal distribution for the background, and give a threshold based on a provided quantile.
* **refine_soma**: find soma based on a starting position by meanshift.
* **integrated**: execute the above procedures sequentially, i.e. denoise -> blur -> max_ds -> auto_thr -> refine_soma
* **guo_enh**: Guo's image enhancement.



Currently no GUI functions are implemented.



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

# auto threshold
v3d -x neuron_image_denoise -f auto_thr -i in.tif -o out.tif -p bg_pct 0.95 norm_qtl 0.95

# meanshift soma refinement
v3d -x neuron_image_denoise -f refine_soma -i in.tif -o out.marker -p \
	start_pos 0.5,0.5,0.5 win_radius 20,20,20 normalized_start true ms_sigma 3,3,3 \
	gsdt true dt_threshold 0 test_count 50 z_thickness 1 connection_type 3

# integrated
vaa3d -x neuron_image_rectify -f integrated -i in.tif -o out.tif out.marker -p [parameters related]

# Guo's image inhancement
v3d -x neuron_image_denoise -f guo_enh -i in.tif -o out.tif -p \
	enh_sigma 1,1,0.25 color_sigma 35 gain 5 cutoff 25 bilateral true fft true
```

