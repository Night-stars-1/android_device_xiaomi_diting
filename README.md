Copyright (C) 2024 The LineageOS Project

Device configuration for Xiaomi 12T Pro / Redmi K50 Ultra
=========================================================

The Xiaomi 12T Pro / Redmi K50 Ultra (codenamed _"diting"_) is a high-end smartphone from Xiaomi.

It was released in China in August 2022 and globally in October 2022.

## Device specifications

Basic   | Spec Sheet
-------:|:-------------------------
Platform | Snapdragon® 8+ Gen 1 (SM8475)
RAM & Storage | 8GB/128GB, 8GB/256GB, 12GB/256GB, 12GB/512GB (LPDDR5 RAM, UFS 3.1 storage)
Shipped Android Version | 12
Battery | Non-removable, 5000 mAh
Display | 6.67″, 120Hz, 2712x1220 (446 ppi)
Rear camera | 200MP (Global) / 108MP (China) wide angle, 8MP ultra wide-angle, 2MP macro
Front camera | 20MP in-display

## Device picture

![Xiaomi 12T Pro](https://i01.appmifile.com/v1/MI_18455B3E4DA706226CF7535A58E875F0267/pms_1666598561.94078322.png "Xiaomi 12T Pro in black")

## MiPush prebuilt packages

`uwu_diting` installs the MiPushFramework normal ARM64 runtime (`com.xiaomi.xmsf`)
and its matching manager (`io.github.magisk317.mipush`) under `/product/app`.
The two APKs use the same dedicated signing key, not the platform or upstream
key. Preserve that key for both APKs across OTA releases. Devices with a
differently signed install of either package must remove the conflicting
package and may need to set up push registration again.

Corresponding source: [MiPushFramework v1.1.0](https://github.com/magisk317/MiPushFramework/tree/177639faf0ee4f0a3a6e24d1708e9526efd9d891),
its pinned submodules, and [`mipush/uwu-focus.patch`](mipush/uwu-focus.patch).
Clone the v1.1.0 tag with submodules, apply the patch with `git apply`, then run
the following in that checkout with JDK 27, Android SDK 37.2, build-tools 37.0.0,
and the existing dedicated key (`KEYSTORE_FILE`, `STORE_PASSWORD`, `KEY_ALIAS`,
`KEY_PASSWORD`):

```sh
./gradlew --no-configuration-cache :xmsf:assembleNormalRelease :mipush:assembleGithubRelease -PbuildSplits=true -PversionCode=1621
```

Use the `arm64-v8a` outputs from both modules. The patch preserves explicit focus
extras when `notification_focus_protocol=3`; it does not enable MIUI proxying or
make ordinary notifications into focus notifications.
