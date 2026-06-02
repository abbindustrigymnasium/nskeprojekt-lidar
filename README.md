# Pan-Tilt 3D Room Scanner

A DIY "LiDAR-style" 3D scanner built from a Time-of-Flight distance sensor on a
pan-tilt servo mount. The scanner sweeps a small scene, measures the distance at
each angle, and renders the result as an interactive 3D point cloud in the browser.

## How It Works

1. Two servos aim the sensor across a grid of angles (pan = horizontal,
   tilt = vertical).
2. At each angle the ToF sensor measures the distance to the nearest surface.
3. Each `(pan, tilt, distance)` reading is converted from spherical to
   cartesian coordinates `(x, y, z)`.
4. The points are drawn as a 3D point cloud you can rotate, zoom, and pan.

## Hardware

- **ESP32-S3** dev board (microcontroller + WiFi)
- **VL53L0X** Time-of-Flight distance sensor
- **2x SG90** servos
- **Pan-tilt mount** (3D printed, based on a Thingiverse design)
- USB cable for power and programming

### Wiring

| Component          | ESP32-S3 pin |
|--------------------|--------------|
| VL53L0X VIN        | 3V3          |
| VL53L0X GND        | GND          |
| VL53L0X SDA        | GPIO 8       |
| VL53L0X SCL        | GPIO 9       |
| Pan servo signal   | GPIO 5       |
| Tilt servo signal  | GPIO 4       |
| Servo VCC (red)    | 5V           |
| Servo GND (brown)  | GND          |

> Servos need 5V on their power line, not 3.3V. All grounds must be shared.

## Software

- **Arduino sketch** (`scanner.ino`) — runs on the ESP32-S3, sweeps the servos,
  reads the sensor, and prints each reading as CSV over the serial port.
- **Viewer** (`viewer.html`) — open in a browser, paste the CSV from the serial
  monitor, and click *Render* to build the 3D point cloud.

### Libraries

Install via the Arduino Library Manager:

- `ESP32Servo` (Kevin Harrington)
- `VL53L0X` (Pololu)

The viewer uses [Three.js](https://threejs.org/) loaded from a CDN — no install
needed.

## Calibration

These values map servo angles to real directions and must match your build:

- **Pan center (straight ahead):** `90` — higher = left, lower = right
- **Tilt horizon (straight ahead, level):** `130` — higher = down, lower = up

If the rendered scene looks mirrored, use the *Flip pan* / *Flip tilt*
checkboxes in the viewer to correct it.

## Usage

1. Flash `scanner.ino` to the ESP32-S3.
2. Place a small scene 0.6–1.5 m directly in front of the scanner.
3. Open the Arduino Serial Monitor at **115200 baud**.
4. Wait for the sweep to finish (between `SCAN_START` and `SCAN_END`).
5. Select all the output, copy it.
6. Open `viewer.html`, paste the data, and click *Render*.

## Limitations

The VL53L0X is a short-range sensor with a wide field of view, which shapes what
this scanner can realistically capture:

- **Range:** reliable only up to ~1.5–2 m. Anything further is dropped.
- **Field of view:** the sensor "sees" a ~25° cone, not a thin beam. Fine detail
  gets smoothed out, so the point cloud is a soft depth shell rather than a sharp
  scan. Angular steps finer than ~10–15° add density but not real detail.
- **Best results:** scan a single, simple object centered and close to the rig,
  not an entire room.

## Possible Improvements

- Stream points over WiFi (WebSocket) so the cloud builds up live during the
  sweep, instead of copy-pasting CSV.
- Add an export button to save the cloud as a `.ply` file for Blender / MeshLab.
- Swap the VL53L0X for a VL53L1X (narrower, adjustable field of view) for sharper
  results.
- Replace the pan servo with a stepper motor for more precise positioning.
