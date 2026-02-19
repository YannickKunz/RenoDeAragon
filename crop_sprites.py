#!/usr/bin/env python3
"""Auto-crop sprite images to remove transparent borders.
For spritesheets, crops each frame to the same bounding box."""

from PIL import Image
import os

SPRITES_DIR = "assets/sprites"

# Single sprites to crop (not backgrounds/fullscreen images)
SINGLE_SPRITES = [
    "dayCharacter.png",
    "deathPlayerPot.png",
    "roach.png",
    "dayWaterPot.png",
    "nightWaterPot.png",
    "mushroomDayUpDown.png",
    "nightShroom.png",
    "player-1.png",
    "flower.png",
    "flower-1.png",
    "newFlower.png",
]

# Spritesheets: (filename, frame_count)
SPRITESHEETS = [
    ("walkingDayCharAnimationSpreadsheet.png", 6),
    ("spiderMoveSpreadsheet.png", 2),
    ("flowerAnimationSpreadsheet.png", 6),
]

def crop_single(filepath):
    """Crop a single sprite to its non-transparent bounding box."""
    img = Image.open(filepath)
    if img.mode != 'RGBA':
        print(f"  Skipping {filepath} - not RGBA (no alpha channel)")
        return
    
    bbox = img.getbbox()
    if bbox is None:
        print(f"  Skipping {filepath} - fully transparent!")
        return
    
    orig_size = img.size
    cropped = img.crop(bbox)
    cropped.save(filepath)
    print(f"  {os.path.basename(filepath)}: {orig_size} -> {cropped.size} (bbox: {bbox})")

def crop_spritesheet(filepath, frame_count):
    """Crop a spritesheet: find max bounding box across all frames, crop all to that."""
    img = Image.open(filepath)
    if img.mode != 'RGBA':
        print(f"  Skipping {filepath} - not RGBA")
        return
    
    frame_w = img.width // frame_count
    frame_h = img.height
    
    # Find bounding box for each frame, then take the union
    min_x, min_y = frame_w, frame_h
    max_x, max_y = 0, 0
    
    for i in range(frame_count):
        frame = img.crop((i * frame_w, 0, (i + 1) * frame_w, frame_h))
        bbox = frame.getbbox()
        if bbox:
            min_x = min(min_x, bbox[0])
            min_y = min(min_y, bbox[1])
            max_x = max(max_x, bbox[2])
            max_y = max(max_y, bbox[3])
    
    if max_x <= min_x or max_y <= min_y:
        print(f"  Skipping {filepath} - no content found")
        return
    
    # Crop each frame to the unified bounding box and reassemble
    new_frame_w = max_x - min_x
    new_frame_h = max_y - min_y
    new_img = Image.new('RGBA', (new_frame_w * frame_count, new_frame_h))
    
    for i in range(frame_count):
        frame = img.crop((i * frame_w + min_x, min_y, i * frame_w + max_x, max_y))
        new_img.paste(frame, (i * new_frame_w, 0))
    
    orig_size = img.size
    new_img.save(filepath)
    print(f"  {os.path.basename(filepath)}: {orig_size} -> {new_img.size} (frame: {frame_w}x{frame_h} -> {new_frame_w}x{new_frame_h})")

if __name__ == "__main__":
    print("=== Cropping single sprites ===")
    for name in SINGLE_SPRITES:
        path = os.path.join(SPRITES_DIR, name)
        if os.path.exists(path):
            crop_single(path)
        else:
            print(f"  Not found: {path}")
    
    print("\n=== Cropping spritesheets ===")
    for name, frames in SPRITESHEETS:
        path = os.path.join(SPRITES_DIR, name)
        if os.path.exists(path):
            crop_spritesheet(path, frames)
        else:
            print(f"  Not found: {path}")
    
    print("\nDone!")
