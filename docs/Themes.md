# Themes
Using themes, the look and feel of Pico Launcher can be customized. Themes are placed in subfolders of the `/_pico/themes` folder. For example `/_pico/themes/my_theme`.

## JSON file
Each theme has a `theme.json` file with information about the theme.

- **type** - Type of theme. Currently `material` and `custom` are supported. See below for information about each type.
- **name** - The name of the theme.
- **description** - Description of the theme.
- **author** - Author of the theme.
- **primaryColor** - Material Design 3 primary color to use. `r`, `g` and `b` are provided in range 0-255.
- **darkTheme** - When `true`, a dark Material Design 3 palette will be used.
- **launchTransition** - Optional launch transition styling shared by material and custom themes. Omit the object or any field to preserve the defaults: `coverStartScalePercent` is `100`, `coverFinalAlpha` is `12`, and `scrimFinalAlpha` is `14`. The scale must be an integer from `1` to `200`; alpha values must be integers from `0` to `31`. A field with the wrong JSON type or an out-of-range value independently falls back to its default.

### Example
```json
{
    "type": "material",
    "name": "Theme name",
    "description": "Theme description here.",
    "author": "Author Name",
    "primaryColor": {
        "r": 149,
        "g": 143,
        "b": 237
    },
    "darkTheme": true,
    "launchTransition": {
        "coverStartScalePercent": 100,
        "coverFinalAlpha": 12,
        "scrimFinalAlpha": 14
    }
}
```

Existing themes do not need to add `launchTransition`; omission preserves the current launch animation exactly. Unknown fields are ignored.

## Startup intro

Place an optional `intro.pani` in the theme folder. To convert an animation:

```bash
python3 -m venv .venv
.venv/bin/pip install -r tools/requirements-intro.txt
.venv/bin/python tools/convert_intro.py source.gif intro.pani
```

The input must be an animated GIF or PNG/APNG with 1-150 frames at exactly 256x192. Every frame must last at least 67 ms, and the total quantized duration must be 0.5-10 seconds. Transparency is composited onto black. The converter uses one global 256-color palette, no dithering, and independent LZ4 blocks stored at 512-byte-aligned offsets with zero padding through each sector. Copy the output to `/_pico/themes/<theme>/intro.pani`.

## Material type
![Horizontal display mode with custom theme](images/Horizontal.png)

The `material` type theme is a pure Material Design 3 theme. It will be fully themed based on the `primaryColor` and `darkTheme` settings from the `theme.json`. In coverflow mode, this theme type uses a Material Design 3 style carousel.

## Custom type
![Horizontal display mode with custom theme](images/HorizontalCustom.png)

The `custom` type theme is much more customizable, compared to the `material` type theme.
Note that the `primaryColor` and `darkTheme` settings from the `theme.json` are still used to color some parts of the UI.

The following additional files are needed:
| Files                                                        | Size                 | Format                   | Description                                                    |
|--------------------------------------------------------------|----------------------|--------------------------|----------------------------------------------------------------|
| bannerListCell.bin<br>bannerListCellPltt.bin                 | 256x49 (209x49 used) | A3I5<br>32 color palette | Unselected item background for banner list mode.               |
| bannerListCellSelected.bin<br>bannerListCellSelectedPltt.bin | 256x49 (209x49 used) | A3I5<br>32 color palette | Selected item background for banner list mode.                 |
| bottombg.bin                                                 | 256x192              | 15 bpp bitmap            | Bottom screen background.                                      |
| gridcell.bin<br>gridcellPltt.bin                             | 64x48 (48x48 used)   | A3I5<br>32 color palette | Unselected item background for grid modes.                     |
| gridcellSelected.bin<br>gridcellPlttSelected.bin             | 64x48 (48x48 used)   | A3I5<br>32 color palette | Selected item background for grid modes.                       |
| scrim.bin<br>scrimPltt.bin                                   | 8x42                 | A5I3<br>8 color palette  | Background for the toolbar. Intended to be a translucent fade. |
| topbg.bin                                                    | 256x192              | 15 bpp bitmap            | Top screen background.                                         |

These files can be created, for example, using [NitroPaint](https://github.com/Garhoogin/NitroPaint).

The top screen background should include a box in which the banner text and icon of the selected item will be shown.

### Additional JSON properties
Custom themes support additional properties in the `theme.json` file to allow for more customization.

- **topIcon** - Properties of the icon displayed on the top screen.
- **topBannerTextLine0** - Properties of the first banner text line displayed on the top screen.
- **topBannerTextLine1** - Properties of the second banner text line displayed on the top screen.
- **topBannerTextLine2** - Properties of the third banner text line displayed on the top screen.
- **topFileNameText** - Properties of the file name text displayed on the top screen.
- **topCover** - Properties of the cover image displayed on the top screen.
- **gridIcon** - Properties of the icons displayed on the bottom screen in grid display modes.
- **bannerListIcon** - Properties of the icons displayed on the bottom screen in banner list display mode.
- **bannerListTextLine0** - Properties of the first banner text line displayed on the bottom screen in banner list display mode.
- **bannerListTextLine1** - Properties of the second banner text line displayed on the bottom screen in banner list display mode.
- **bannerListTextLine2** - Properties of the third banner text line displayed on the bottom screen in banner list display mode.

Blend colors are used to fake translucency. They should be set to an approximation of the background color.

```json
{
    "type": "custom",
    "name": "Raspberry",
    "description": "Theme based on raspberries.",
    "author": "Gericom",
    "primaryColor": { "r": 138, "g": 217, "b": 255 },
    "darkTheme": false,
    "topIcon": {
        "position": { "x": 24, "y": 132 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topBannerTextLine0": {
        "position": { "x": 70, "y": 126 },
        "width": 168,
        "textColor": { "r": 30, "g": 30, "b": 30 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topBannerTextLine1": {
        "position": { "x": 70, "y": 141 },
        "width": 168,
        "textColor": { "r": 30, "g": 30, "b": 30 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topBannerTextLine2": {
        "position": { "x": 70, "y": 155 },
        "width": 168,
        "textColor": { "r": 30, "g": 30, "b": 30 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topFileNameText": {
        "position": { "x": 18, "y": 170 },
        "width": 220,
        "textColor": { "r": 30, "g": 30, "b": 30 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topCover": {
        "position": { "x": 75, "y": 18 }
    },
    "gridIcon": {
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "bannerListIcon": {
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "bannerListTextLine0": {
        "textColor": { "r": 30, "g": 30, "b": 30 }
    },
    "bannerListTextLine1": {
        "textColor": { "r": 30, "g": 30, "b": 30 }
    },
    "bannerListTextLine2": {
        "textColor": { "r": 30, "g": 30, "b": 30 }
    }
}
```

## Background music
All themes support background music by placing DSP-ADPCM encoded `.bcstm` files in a `bgm` folder inside the theme folder. Looping is supported. When multiple `.bcstm` files are provided, the background music will be selected at random each time Pico Launcher is started.

## UI sounds
All themes can provide optional UI sounds in a `sounds` folder inside the theme folder. Missing or invalid files are ignored.

| File                    | Event                                                                 | Recommended duration |
|-------------------------|-----------------------------------------------------------------------|----------------------|
| `sounds/navigation.wav` | The selected game changes in any catalog layout.                      | 50-150 ms            |
| `sounds/launch.wav`     | A game launch begins; playback overlaps the existing screen fade-out. | 150-250 ms           |

Both WAV files must use uncompressed PCM, signed 16-bit little-endian samples, one channel (mono), and a 22050 Hz sample rate. The PCM data in each file may not exceed 11,024 bytes (about 250 ms), and each complete WAV file may not exceed 16 KiB. These one-shot sounds share one audio channel, so a launch sound replaces any navigation sound that is still playing. Streamed `.bcstm` background music remains separate under `bgm`.

## Theme selector icon
A theme can have an `icon.bmp` file that is shown in the theme list when selecting a theme. It must be **32×32 pixels, 4 bpp (16 colors), uncompressed `.bmp`** file, with the first palette color treated as transparent.

## Theme selector preview image
A theme can have a `preview.bin` file that is shown on the top screen in the theme selection screen. It must be a 256x192 pixels 15 bpp bitmap (same format as `topbg.bin` and `bottombg.bin`). When it is not provided, `topbg.bin` is displayed instead. If that does not exist either, nothing is shown on the top screen.
