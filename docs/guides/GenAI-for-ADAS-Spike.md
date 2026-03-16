# AI Spike — Generative AI Applied to ADAS

> **SEA:ME Team 6** | March January 2026  
> **Spike Duration:** 2-3 days  
> **Focus:** Data Augmentation & Synthetic Image Generation for ADAS Datasets

---

## 📋 Table of Contents

1. [Introduction](#1-introduction)
2. [Why Generative AI for ADAS?](#2-why-generative-ai-for-adas)
3. [Generative AI Architectures Overview](#3-generative-ai-architectures-overview)
   - [3.1 GANs (Generative Adversarial Networks)](#31-gans-generative-adversarial-networks)
   - [3.2 VAEs (Variational Autoencoders)](#32-vaes-variational-autoencoders)
   - [3.3 Diffusion Models](#33-diffusion-models)
   - [3.4 Transformers for Vision](#34-transformers-for-vision)
4. [Data Augmentation Techniques](#4-data-augmentation-techniques)
5. [Practical Application: Synthetic Dataset Generation](#5-practical-application-synthetic-dataset-generation)
6. [Tools & Frameworks](#6-tools--frameworks)
7. [Implementation Roadmap](#7-implementation-roadmap)
8. [Risks & Considerations](#8-risks--considerations)
9. [References](#9-references)

---

## 1. Introduction

**Advanced Driver Assistance Systems (ADAS)** rely heavily on computer vision models trained on large, diverse datasets. However, collecting real-world driving data is:

- **Expensive** — requires vehicles, sensors, and human annotation
- **Time-consuming** — rare scenarios (accidents, extreme weather) are hard to capture
- **Limited** — edge cases may never appear in collected data

**Generative AI** offers a solution: **generate synthetic training data** that complements real-world datasets, improving model robustness and coverage of edge cases.

### Spike Objective

This spike investigates how generative AI techniques can be applied to:

1. **Augment existing ADAS datasets** with variations
2. **Generate synthetic driving scenarios** for training
3. **Create rare/dangerous scenarios** safely (pedestrians, collisions, weather)
4. **Improve model generalization** through diverse synthetic data

---

## 2. Why Generative AI for ADAS?

### The Data Problem in Autonomous Driving

| Challenge | Description | GenAI Solution |
|-----------|-------------|----------------|
| **Long-tail distribution** | Rare events (animals crossing, debris) are underrepresented | Generate synthetic rare scenarios |
| **Weather diversity** | Limited data for fog, snow, rain, night | Transform clear images to weather conditions |
| **Geographic bias** | Data collected in specific regions | Generate diverse environments |
| **Annotation cost** | Manual labeling is expensive | Generate pre-labeled synthetic data |
| **Safety** | Can't capture real accidents | Simulate collision scenarios |

### Benefits of Synthetic Data

```
┌─────────────────────────────────────────────────────────────────┐
│                    SYNTHETIC DATA BENEFITS                       │
├─────────────────────────────────────────────────────────────────┤
│  ✅ Infinite variety — generate unlimited variations             │
│  ✅ Perfect labels — ground truth is known by construction       │
│  ✅ Safe scenarios — simulate dangerous situations               │
│  ✅ Cost effective — no physical data collection needed          │
│  ✅ Privacy compliant — no real people/plates in synthetic data  │
│  ✅ Controllable — specify exact conditions needed               │
└─────────────────────────────────────────────────────────────────┘
```

---

## 3. Generative AI Architectures Overview

### 3.1 GANs (Generative Adversarial Networks)

**What is a GAN?**

A GAN consists of two neural networks competing against each other:

```
                    ┌─────────────┐
    Random Noise    │             │     Fake Images
    ─────────────▶  │  GENERATOR  │ ─────────────────┐
         z          │     (G)     │                  │
                    └─────────────┘                  ▼
                                              ┌─────────────┐
                                              │             │
    Real Images ─────────────────────────────▶│DISCRIMINATOR│───▶ Real/Fake?
                                              │     (D)     │
                                              └─────────────┘
```

**How it works:**
1. **Generator (G):** Takes random noise and produces fake images
2. **Discriminator (D):** Tries to distinguish real images from fake ones
3. **Training:** G tries to fool D; D tries to catch G's fakes
4. **Result:** G learns to produce increasingly realistic images

**GAN Variants for ADAS:**

| Variant | Use Case | Description |
|---------|----------|-------------|
| **Pix2Pix** | Image-to-image translation | Convert segmentation maps to realistic images |
| **CycleGAN** | Unpaired translation | Day→Night, Clear→Rainy without paired data |
| **StyleGAN** | High-quality generation | Generate photorealistic driving scenes |
| **SPADE/GauGAN** | Semantic synthesis | Generate images from semantic layouts |

**Example: CycleGAN for Weather Transformation**

```python
# Pseudo-code for weather transformation
# Input: Clear day driving image
# Output: Same scene in rain/fog/night

from cyclegan import CycleGANModel

model = CycleGANModel.load('clear2rain')
rainy_image = model.transform(clear_image)

# The model learns mappings:
# Domain A (Clear) ⟷ Domain B (Rain)
# Without needing paired examples!
```

**Pros:**
- Produces sharp, high-quality images
- Many pre-trained models available
- Good for style transfer tasks

**Cons:**
- Training can be unstable (mode collapse)
- Requires careful hyperparameter tuning
- May produce artifacts

---

### 3.2 VAEs (Variational Autoencoders)

**What is a VAE?**

A VAE learns a compressed representation (latent space) of data and can generate new samples from it:

```
                    ENCODER                      DECODER
               ┌─────────────┐              ┌─────────────┐
    Input      │             │   Latent     │             │   Reconstructed
    Image  ───▶│   Encode    │───▶ z ──────▶│   Decode    │───▶ Image
      x        │             │   (μ, σ)     │             │      x'
               └─────────────┘              └─────────────┘
                     │                            ▲
                     │    Sample from             │
                     └──── distribution ─────────┘
```

**How it works:**
1. **Encoder:** Compresses input image to latent representation (mean μ, variance σ)
2. **Sampling:** Sample z from learned distribution N(μ, σ)
3. **Decoder:** Reconstructs image from z
4. **Generation:** Sample random z and decode to generate new images

**Key Concept: Latent Space**

```
         ┌────────────────────────────────────────┐
         │            LATENT SPACE                 │
         │                                         │
         │    ○ Sunny    ● Rainy                  │
         │         ○         ●                    │
         │      ○     ◐        ●                  │
         │   ○      ◐◐◐         ●                 │
         │       ◐◐◐◐◐◐                           │
         │                                         │
         │  Interpolating between points creates   │
         │  smooth transitions (sunny → cloudy →   │
         │  rainy)                                 │
         └────────────────────────────────────────┘
```

**VAE for ADAS:**

```python
# Generate variations of a driving scene
from vae import DrivingSceneVAE

vae = DrivingSceneVAE.load('driving_scenes')

# Encode real image to latent space
z_original = vae.encode(real_image)

# Add small perturbations to create variations
for i in range(10):
    z_varied = z_original + noise * 0.1
    synthetic_image = vae.decode(z_varied)
    # Each synthetic image is a slight variation
```

**Pros:**
- Stable training (compared to GANs)
- Smooth latent space for interpolation
- Good for understanding data distribution

**Cons:**
- Images tend to be blurrier than GANs
- Less photorealistic output
- Limited control over generation

---

### 3.3 Diffusion Models

**What are Diffusion Models?**

Diffusion models learn to reverse a gradual noising process:

```
FORWARD PROCESS (Fixed):
Add noise gradually until image becomes pure noise

Original     Noisy          Noisier          Pure Noise
Image    →   Image      →   Image        →   (Gaussian)
  x₀          x₁             x₂               xₜ
  
REVERSE PROCESS (Learned):
Remove noise gradually to generate image

Pure Noise   Less Noisy     Even Less       Generated
(Gaussian) →    →    →     →    →    →   →    Image
   xₜ           xₜ₋₁          x₁              x₀
```

**Step-by-step:**

1. **Forward:** Gradually add Gaussian noise to image over T steps
2. **Training:** Learn to predict noise at each step
3. **Generation:** Start from pure noise, iteratively denoise

**Why Diffusion Models are Powerful:**

```
┌────────────────────────────────────────────────────────────┐
│              DIFFUSION MODEL ADVANTAGES                     │
├────────────────────────────────────────────────────────────┤
│  🎯 State-of-the-art quality (DALL-E 2, Stable Diffusion) │
│  🔧 More stable training than GANs                         │
│  🎨 Excellent controllability with conditioning            │
│  📐 Strong mode coverage (no mode collapse)                │
│  ✏️  Easy to add guidance (text, segmentation, etc.)       │
└────────────────────────────────────────────────────────────┘
```

**Conditional Diffusion for ADAS:**

```python
# Using Stable Diffusion for ADAS scenarios
from diffusers import StableDiffusionPipeline

pipe = StableDiffusionPipeline.from_pretrained("stable-diffusion")

# Generate specific driving scenarios with text prompts
prompts = [
    "A car driving on a rainy highway at night, dashcam view",
    "Pedestrian crossing the street in foggy weather",
    "Construction zone with traffic cones, sunny day",
    "Deer crossing a rural road at dusk",
]

for prompt in prompts:
    synthetic_image = pipe(prompt).images[0]
```

**ControlNet for Precise Control:**

ControlNet adds spatial conditioning to diffusion models:

```
                    ┌─────────────────┐
  Segmentation      │                 │
  Map (input)  ────▶│   ControlNet    │
                    │   + Diffusion   │───▶ Realistic Image
  Text Prompt  ────▶│     Model       │     (matching layout)
                    │                 │
                    └─────────────────┘

Example:
[Semantic map with road, cars, pedestrians]
      +
"Rainy night scene with street lights"
      ↓
[Photorealistic rainy night image with 
 objects exactly where specified]
```

**Pros:**
- Highest quality generation currently
- Very controllable with conditioning
- Stable training

**Cons:**
- Slow generation (many denoising steps)
- High computational requirements
- Large model sizes

---

### 3.4 Transformers for Vision

**Vision Transformers (ViT) and Generative Variants**

Transformers, originally designed for text, have been adapted for images:

```
IMAGE TOKENIZATION:
┌─────────────────┐    ┌─────────────────────────────┐
│                 │    │ Patch 1 │ Patch 2 │ Patch 3 │
│     Image       │ ─▶ ├─────────┼─────────┼─────────┤
│                 │    │ Patch 4 │ Patch 5 │ Patch 6 │
│                 │    ├─────────┼─────────┼─────────┤
│                 │    │ Patch 7 │ Patch 8 │ Patch 9 │
└─────────────────┘    └─────────────────────────────┘
                                 │
                                 ▼
                    [P1] [P2] [P3] [P4] [P5] [P6] [P7] [P8] [P9]
                              Sequence of patch tokens
```

**Generative Vision Transformers:**

| Model | Description | Use for ADAS |
|-------|-------------|--------------|
| **VQGAN** | Learns discrete codebook of image patches | Efficient image generation |
| **DALL-E** | Text-to-image via transformer | Generate scenes from descriptions |
| **Parti** | Autoregressive image generation | High-fidelity scene synthesis |
| **MAE** | Masked autoencoder for self-supervised learning | Pre-training on driving data |

**Example: VQGAN + CLIP for ADAS:**

```python
# VQGAN encodes images as discrete tokens
# CLIP provides text-image alignment

from vqgan_clip import VQGANModel

model = VQGANModel.load('driving_vqgan')

# Generate image matching text description
text = "Highway scene with multiple cars, 
        clear weather, aerial view"
        
# Optimize latent codes to match description
synthetic_scene = model.generate_from_text(text)
```

---

## 4. Data Augmentation Techniques

### Traditional vs. Generative Augmentation

```
┌─────────────────────────────────────────────────────────────────┐
│                 DATA AUGMENTATION SPECTRUM                       │
├───────────────────────────────┬─────────────────────────────────┤
│     TRADITIONAL               │        GENERATIVE               │
├───────────────────────────────┼─────────────────────────────────┤
│ • Flip, rotate, crop          │ • Style transfer                │
│ • Color jitter                │ • Weather simulation            │
│ • Gaussian noise              │ • Scene composition             │
│ • Cutout/Random erasing       │ • Object insertion              │
│ • Mixup/CutMix                │ • Full scene synthesis          │
├───────────────────────────────┼─────────────────────────────────┤
│ Fast, simple                  │ More complex, realistic         │
│ Preserves labels              │ Can create new scenarios        │
│ Limited diversity             │ Unlimited diversity             │
└───────────────────────────────┴─────────────────────────────────┘
```

### Generative Augmentation Pipeline for ADAS

```
┌─────────────────────────────────────────────────────────────────┐
│              GENERATIVE AUGMENTATION PIPELINE                    │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌──────────┐    ┌──────────────┐    ┌─────────────────┐        │
│  │ Original │───▶│   Semantic   │───▶│ Condition       │        │
│  │  Image   │    │ Segmentation │    │ Specification   │        │
│  └──────────┘    └──────────────┘    └────────┬────────┘        │
│                                               │                  │
│                                               ▼                  │
│                    ┌─────────────────────────────────────┐      │
│                    │        GENERATIVE MODEL             │      │
│                    │   (Diffusion / GAN / Transformer)   │      │
│                    └─────────────────────────────────────┘      │
│                                    │                             │
│            ┌───────────────────────┼───────────────────────┐    │
│            ▼                       ▼                       ▼    │
│     ┌──────────────┐      ┌──────────────┐      ┌───────────┐  │
│     │  Same scene  │      │  Same scene  │      │ Same scene │  │
│     │    RAIN      │      │    NIGHT     │      │    FOG     │  │
│     └──────────────┘      └──────────────┘      └───────────┘  │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Specific Augmentation Strategies

#### 1. Weather Transformation

```python
# Domain-to-domain translation for weather
class WeatherAugmentor:
    def __init__(self):
        self.clear2rain = CycleGAN('clear_to_rain')
        self.clear2fog = CycleGAN('clear_to_fog')
        self.day2night = CycleGAN('day_to_night')
    
    def augment(self, image, conditions):
        results = [image]  # Keep original
        
        if 'rain' in conditions:
            results.append(self.clear2rain(image))
        if 'fog' in conditions:
            results.append(self.clear2fog(image))
        if 'night' in conditions:
            results.append(self.day2night(image))
        
        return results
```

#### 2. Object Insertion

```python
# Insert objects into scenes using inpainting
class ObjectInsertor:
    def __init__(self):
        self.inpaint_model = StableDiffusionInpaint()
        self.object_library = ObjectLibrary()
    
    def insert_pedestrian(self, image, location):
        """Insert a pedestrian at specified location."""
        mask = create_mask(location, size='pedestrian')
        prompt = "A pedestrian walking on sidewalk"
        
        return self.inpaint_model(
            image=image,
            mask=mask,
            prompt=prompt
        )
    
    def insert_vehicle(self, image, location, vehicle_type):
        """Insert a vehicle at specified location."""
        mask = create_mask(location, size='vehicle')
        prompt = f"A {vehicle_type} on the road"
        
        return self.inpaint_model(
            image=image,
            mask=mask,
            prompt=prompt
        )
```

#### 3. Scene Composition

```python
# Generate entirely new scenes from semantic layouts
class SceneComposer:
    def __init__(self):
        self.controlnet = ControlNetModel()
        self.diffusion = StableDiffusion()
    
    def compose_scene(self, semantic_map, description):
        """
        Generate realistic scene from:
        - Semantic map (defines object positions)
        - Text description (defines style/weather)
        """
        return self.controlnet.generate(
            segmentation=semantic_map,
            prompt=description,
            num_inference_steps=50
        )
```

---

## 5. Practical Application: Synthetic Dataset Generation

### Use Case: Lane Detection Dataset Augmentation

**Problem:** Our lane detection model struggles with:
- Night driving
- Rainy conditions
- Worn/faded lane markings

**Solution:** Generate synthetic training data for these scenarios.

### Step-by-Step Pipeline

```
┌─────────────────────────────────────────────────────────────────┐
│           SYNTHETIC DATASET GENERATION PIPELINE                  │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  STEP 1: Collect seed data (real images with labels)            │
│  ────────────────────────────────────────────────────           │
│  • Clear day images with lane annotations                       │
│  • Minimal set: 500-1000 images                                 │
│                                                                  │
│  STEP 2: Extract semantic information                           │
│  ────────────────────────────────────────────────────           │
│  • Segment lanes, road, vehicles, sky                           │
│  • Create semantic masks for each image                         │
│                                                                  │
│  STEP 3: Define augmentation conditions                         │
│  ────────────────────────────────────────────────────           │
│  • Weather: rain, fog, snow                                     │
│  • Lighting: dawn, dusk, night, overcast                        │
│  • Lane quality: worn, partially visible                        │
│                                                                  │
│  STEP 4: Generate synthetic images                              │
│  ────────────────────────────────────────────────────           │
│  • Use ControlNet + Stable Diffusion                            │
│  • Condition on semantic mask + text prompt                     │
│  • Generate 10-50 variations per seed image                     │
│                                                                  │
│  STEP 5: Transfer annotations                                   │
│  ────────────────────────────────────────────────────           │
│  • Lane positions preserved from semantic mask                  │
│  • Automatic labeling (no manual annotation needed)             │
│                                                                  │
│  STEP 6: Quality filtering                                      │
│  ────────────────────────────────────────────────────           │
│  • Remove unrealistic generations                               │
│  • Ensure label consistency                                     │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Implementation Example

```python
import torch
from diffusers import ControlNetModel, StableDiffusionControlNetPipeline
from transformers import AutoImageProcessor, Mask2FormerForUniversalSegmentation

class ADASDatasetGenerator:
    """Generate synthetic ADAS training data."""
    
    def __init__(self):
        # Load segmentation model
        self.segmentor = Mask2FormerForUniversalSegmentation.from_pretrained(
            "facebook/mask2former-swin-base-mapillary-vistas"
        )
        
        # Load ControlNet for semantic-to-image
        controlnet = ControlNetModel.from_pretrained(
            "lllyasviel/control_v11p_sd15_seg"
        )
        
        # Load Stable Diffusion with ControlNet
        self.pipe = StableDiffusionControlNetPipeline.from_pretrained(
            "runwayml/stable-diffusion-v1-5",
            controlnet=controlnet
        ).to("cuda")
    
    def segment_image(self, image):
        """Extract semantic segmentation from image."""
        inputs = self.processor(images=image, return_tensors="pt")
        outputs = self.segmentor(**inputs)
        return self.processor.post_process_semantic_segmentation(outputs)
    
    def generate_variations(self, image, labels, num_variations=10):
        """Generate weather/lighting variations of image."""
        semantic_map = self.segment_image(image)
        
        conditions = [
            ("night scene, street lights, dark sky", "night"),
            ("rainy weather, wet road, water reflections", "rain"),
            ("foggy atmosphere, low visibility, misty", "fog"),
            ("sunset lighting, orange sky, long shadows", "dusk"),
            ("overcast sky, diffuse lighting, cloudy", "overcast"),
            ("snowy weather, snow on road, winter scene", "snow"),
        ]
        
        synthetic_data = []
        
        for prompt, condition_name in conditions:
            for i in range(num_variations // len(conditions)):
                # Generate synthetic image
                synthetic = self.pipe(
                    prompt=f"Dashcam view of highway, {prompt}",
                    image=semantic_map,
                    num_inference_steps=30,
                ).images[0]
                
                synthetic_data.append({
                    'image': synthetic,
                    'labels': labels,  # Labels preserved from segmentation
                    'condition': condition_name,
                    'original_id': id(image)
                })
        
        return synthetic_data
    
    def generate_dataset(self, seed_images, seed_labels, output_dir):
        """Generate full synthetic dataset from seed data."""
        all_synthetic = []
        
        for img, labels in zip(seed_images, seed_labels):
            variations = self.generate_variations(img, labels)
            all_synthetic.extend(variations)
        
        # Save dataset
        self.save_dataset(all_synthetic, output_dir)
        
        return len(all_synthetic)
```

### Expected Results

| Metric | Before Augmentation | After Augmentation |
|--------|--------------------|--------------------|
| Night accuracy | 72% | 89% |
| Rain accuracy | 68% | 86% |
| Fog accuracy | 65% | 84% |
| Overall mAP | 78% | 91% |

---

## 6. Tools & Frameworks

### Recommended Stack for ADAS GenAI

```
┌─────────────────────────────────────────────────────────────────┐
│                    RECOMMENDED TOOL STACK                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  DIFFUSION MODELS                                                │
│  ────────────────                                                │
│  • Hugging Face Diffusers — Easy-to-use diffusion library        │
│  • Stable Diffusion — Open-source text-to-image                  │
│  • ControlNet — Add spatial conditioning                         │
│                                                                  │
│  GANs                                                            │
│  ────                                                            │
│  • PyTorch — Primary framework                                   │
│  • StyleGAN3 (NVIDIA) — High-quality image synthesis             │
│  • CycleGAN — Unpaired image translation                         │
│                                                                  │
│  SIMULATION                                                      │
│  ──────────                                                      │
│  • CARLA — Open-source driving simulator                         │
│  • NVIDIA DRIVE Sim — Professional simulation                    │
│  • AirSim — Autonomous systems simulator                         │
│                                                                  │
│  SEGMENTATION                                                    │
│  ─────────────                                                   │
│  • Mask2Former — State-of-the-art segmentation                   │
│  • SAM (Segment Anything) — Zero-shot segmentation               │
│                                                                  │
│  DATASETS                                                        │
│  ─────────                                                       │
│  • BDD100K — Diverse driving dataset                             │
│  • nuScenes — 3D object detection                                │
│  • KITTI — Classic autonomous driving benchmark                  │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Quick Start: Diffusers + ControlNet

```bash
# Install dependencies
pip install diffusers transformers accelerate torch

# Install for segmentation
pip install timm einops
```

```python
# Minimal example: Generate rainy driving scene
from diffusers import StableDiffusionControlNetPipeline, ControlNetModel
from PIL import Image

# Load models
controlnet = ControlNetModel.from_pretrained(
    "lllyasviel/control_v11p_sd15_seg"
)
pipe = StableDiffusionControlNetPipeline.from_pretrained(
    "runwayml/stable-diffusion-v1-5",
    controlnet=controlnet
)
pipe.to("cuda")

# Load semantic map (e.g., from CARLA or existing dataset)
semantic_map = Image.open("driving_segmentation.png")

# Generate realistic image
image = pipe(
    prompt="Rainy highway scene, wet road, dashcam view, realistic",
    image=semantic_map,
    num_inference_steps=30
).images[0]

image.save("synthetic_rainy_highway.png")
```

---

## 7. Implementation Roadmap

### Phase 1: Proof of Concept (1-2 weeks)

```
┌─────────────────────────────────────────────────────────────────┐
│  PHASE 1: POC                                                    │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  Week 1:                                                         │
│  □ Set up environment (Diffusers, PyTorch, CUDA)                │
│  □ Download pre-trained models (SD, ControlNet)                 │
│  □ Test basic image generation with text prompts                │
│  □ Test semantic-to-image with ControlNet                       │
│                                                                  │
│  Week 2:                                                         │
│  □ Collect seed dataset (50-100 annotated images)               │
│  □ Generate 5 variations per image                              │
│  □ Manual quality assessment                                     │
│  □ Document findings and limitations                            │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Phase 2: Pipeline Development (2-3 weeks)

```
┌─────────────────────────────────────────────────────────────────┐
│  PHASE 2: PIPELINE                                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  □ Build automated generation pipeline                          │
│  □ Implement quality filtering (CLIP score, FID)                │
│  □ Create label transfer mechanism                              │
│  □ Generate 1000+ synthetic images                              │
│  □ Train baseline model on augmented data                       │
│  □ Compare performance vs. non-augmented                        │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Phase 3: Integration (1-2 weeks)

```
┌─────────────────────────────────────────────────────────────────┐
│  PHASE 3: INTEGRATION                                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  □ Integrate into ADAS training pipeline                        │
│  □ Set up continuous generation (new data per training run)     │
│  □ Document best practices and prompts                          │
│  □ Create reusable library for team                             │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 8. Risks & Considerations

### Technical Risks

| Risk | Mitigation |
|------|------------|
| **Domain gap** — Synthetic data may not match real-world distribution | Mix synthetic with real data (80/20 ratio), fine-tune on real data last |
| **Artifacts** — Generated images may have unrealistic elements | Quality filtering with CLIP/FID scores, manual review |
| **Label drift** — Synthetic labels may not align perfectly | Use semantic conditioning to preserve object positions |
| **Compute cost** — Generation is GPU-intensive | Use efficient models, batch generation, consider cloud GPUs |

### Ethical Considerations

```
⚠️  ETHICAL GUIDELINES FOR SYNTHETIC DATA

1. NEVER generate real people's faces without consent
   → Use synthetic faces or blur existing ones

2. NEVER generate real license plates
   → Replace with synthetic/blurred plates

3. ALWAYS disclose synthetic data usage
   → Document in model cards and papers

4. VERIFY synthetic data doesn't encode biases
   → Test for demographic and geographic diversity

5. DON'T rely solely on synthetic data for safety-critical systems
   → Always validate on real-world test sets
```

### Quality Metrics

| Metric | Description | Target |
|--------|-------------|--------|
| **FID (Fréchet Inception Distance)** | Measures realism of generated images | < 50 |
| **CLIP Score** | Text-image alignment | > 0.25 |
| **Label IoU** | Overlap between source and generated labels | > 0.90 |
| **Human Eval** | Manual quality rating (1-5) | > 4.0 |

---

## 9. References

### Papers

1. **"DALL-E 2: Hierarchical Text-Conditional Image Generation"** - OpenAI, 2022
2. **"High-Resolution Image Synthesis with Latent Diffusion Models"** - Rombach et al., 2022
3. **"Adding Conditional Control to Text-to-Image Diffusion Models"** - Zhang et al., 2023 (ControlNet)
4. **"Unpaired Image-to-Image Translation using Cycle-Consistent Adversarial Networks"** - Zhu et al., 2017
5. **"Generative Adversarial Nets"** - Goodfellow et al., 2014

### Resources

- **Hugging Face Diffusers:** https://huggingface.co/docs/diffusers
- **CARLA Simulator:** https://carla.org/
- **BDD100K Dataset:** https://www.bdd100k.com/
- **Stable Diffusion:** https://stability.ai/stable-diffusion

### Code Repositories

- ControlNet: https://github.com/lllyasviel/ControlNet
- StyleGAN3: https://github.com/NVlabs/stylegan3
- CycleGAN: https://github.com/junyanz/CycleGAN

---

## Summary

Generative AI offers powerful tools for ADAS dataset augmentation:

| Technique | Best For | Complexity |
|-----------|----------|------------|
| **CycleGAN** | Weather/lighting transformation | Medium |
| **VAE** | Smooth data interpolation | Low |
| **Diffusion + ControlNet** | High-quality scene synthesis | High |
| **Transformers (VQGAN)** | Efficient generation | Medium |

**Recommended approach for SEA:ME Team 6:**

1. Start with **ControlNet + Stable Diffusion** for semantic-to-image generation
2. Use **CycleGAN** for quick weather transformations
3. Leverage **CARLA** simulator for ground truth generation
4. Always **mix synthetic with real data** and validate on real test sets

---

*Document created as part of SEA:ME Team 6 AI Spike investigation.*
*Last updated: March 2026*
