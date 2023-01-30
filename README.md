# Neuron Image Rectify
Preprocessing the neuron image signals to make it benign for reconstruction.

## Commands

* **high_pass_filter**: make neuron fibers standing out and remove flare
* **downsampling**: down sample based on max
* **enhancement**: Guo's image enhancement
* **sparse_auto_thr**: automatic thresholding using percentile and triangle thresholding
* **thr_hist_eq**: given a lower threshold, apply histogram equalization
* **meanshift_soma_refinement**: find soma based on a starting position by meanshift
* **integrated**: execute the above procedures sequentially

## Usage

Currently no GUI functions are implemented.

CLI functions can be parameterized through `-p`, but different from other Vaa3D plugins, in that you can specify them as `<Name> <Value>` pairs.

```bash
# high pass filter
vaa3d -x neuron_image_rectify -f high_pass_filter -i in.tif -o out.tif -p hp_sigma 8,8,2 hp_skew 0,0,-1
# downsampling
vaa3d -x neuron_image_rectify -f downsampling -i in.tif -o out.tif -p scale 4,4,1
# enhancement
vaa3d -x neuron_image_rectify -f enhancement -i in.tif -o out.tif -p enh_sigma 1,1,0.25 color_sigma 35 gain 5 cutoff 25 bilateral true fft true
# sparse_auto_thr
vaa3d -x neuron_image_rectify -f sparse_auto_thr -i in.tif -o out.marker -p fg_percentile 0.01 n_bin_float 256
# thr_hist_eq
vaa3d -x neuron_image_rectify -f thr_hist_eq -i in.tif -o out.tif out.marker -p hist_eq_thr 0 dtype 1 n_bin_float 256
# meanshift_soma_refinement
vaa3d -x neuron_image_rectify -f meanshift_soma_refinement -i in.tif -o out.marker -p start_pos 0.5,0.5,0.5 win_radius 20,20,20 normalized_start true ms_sigma 3,3,3 gsdt true dt_threshold 0 test_count 50 z_thickness 1 connection_type 3
# integrated
vaa3d -x neuron_image_rectify -f integrated -i in.tif -o out.tif out.marker -p [any parameter above]
```

