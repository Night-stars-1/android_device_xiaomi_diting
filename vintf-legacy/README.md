# diting FCM 6 compatibility

`compatibility_matrix.6.xml` is the complete, unmodified AOSP framework FCM 6
matrix from `platform/hardware/interfaces`, commit
`5688f7eb1e117ed26e642a695de300b7683acb87` (`android16-release`):

[Upstream source](https://android.googlesource.com/platform/hardware/interfaces/+/5688f7eb1e117ed26e642a695de300b7683acb87/compatibility_matrices/compatibility_matrix.6.xml).

The module combines it with the source tree's `kernel_config_s_5.10`, including
its base and conditional requirements. It installs to the system partition only
when selected by diting. It does not change the vendor manifest's target level,
kernel release/KMI metadata, shipping API, or VINTF enforcement.

The current Android 17 tree removed the FCM 6 system matrix, while diting's four
vendor SKU manifests and Android 12 kernel still declare level 6. With the
additional matrix, the real host VINTF checker accepted all four SKU combinations
against the 2026-10-15 target-files in an isolated preview. Source integration,
rebuilt image packaging and device operation require subsequent validation.
