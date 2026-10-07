#!/usr/bin/env python3
import json
from pathlib import Path
from PIL import Image, ImageFilter

def apply_privacy_blur():
    base_dir = Path("docs/images")
    coords_file = base_dir / "blur_coords.json"
    coords = {}
    if coords_file.exists():
        with open(coords_file, "r", encoding="utf-8") as f:
            coords = json.load(f)

    def blur_image(raw_name, out_name, box_key):
        raw_path = base_dir / raw_name
        if not raw_path.exists():
            return
        im = Image.open(raw_path).convert("RGBA")
        for box in coords.get(box_key, []):
            x, y, w, h = box["x"], box["y"], box["width"], box["height"]
            crop_box = (x, y - 1, x + w, y + h + 1)
            region = im.crop(crop_box)
            blurred = region.filter(ImageFilter.GaussianBlur(radius=6))
            im.paste(blurred, crop_box)
        out_path = base_dir / out_name
        im.convert("RGB").save(out_path, "PNG", optimize=True)
        print(f"Salvato {out_path}")

    # 1. Main IT & Options IT
    blur_image("raw_screenshot_main_it.png", "screenshot_main_it.png", "it_main")
    blur_image("raw_screenshot_options_it.png", "screenshot_options_it.png", "it_options")

    # 2. Guide IT (senza dati sensibili, solo conversione ottimizzata)
    im_guide_it = Image.open(base_dir / "raw_screenshot_guide_it.png").convert("RGB")
    im_guide_it.save(base_dir / "screenshot_guide_it.png", "PNG", optimize=True)
    print("Salvato docs/images/screenshot_guide_it.png")

    # 3. Main EN & Options EN
    blur_image("raw_screenshot_main_en.png", "screenshot_main_en.png", "en_main")
    blur_image("raw_screenshot_options_en.png", "screenshot_options_en.png", "en_options")

    # 4. Guide EN (senza dati sensibili, solo conversione ottimizzata)
    im_guide_en = Image.open(base_dir / "raw_screenshot_guide_en.png").convert("RGB")
    im_guide_en.save(base_dir / "screenshot_guide_en.png", "PNG", optimize=True)
    print("Salvato docs/images/screenshot_guide_en.png")

if __name__ == "__main__":
    apply_privacy_blur()
