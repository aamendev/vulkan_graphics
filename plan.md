# Todo
- Texture;

--> Create two private functions in Renderer: beginSingleTimeCommands and endSingleTimeCommands
--> modify copyBuffer code to be from the renderer, rather than the buffer.
-->

-> Renderer should create a texture when requested.
-> it should also handle all processing stuff (e.g. transitioing layout and stuff)
-> Renderer can also copy data between its children. No child can transfer copies to any other child except through renderer (i.e. no buffer-> image, image -> image or buffer-> buffer communication)

# Current Issues

## Device dependency
- Too many friend classes to device handler -> Every class must have device as one of its arguments
### Possible solutions
- Create a base Graphics Class that includes pointer to device/device_handler and make all classes inherit from it;

# General Design

- Allocate insane memory;
- App run;
- Clean memory;

## App run

- init root (for now, window, renderer, etc...)
- main loop;
- cleanup; (maybe later)

### init root

- init everything else;
- root shares pointers to managers which handle separate components
- these managers (e.g. renderer, sound manager) should potentially run on separate threads.

### main loop

- load graphics data (positions, textures, etc...)
- beginDraw();
- set draw settings;
- endDraw();

# Main Classes:

- Buffer (General, staging, index, command, uniform, vertex);
- Texture;
- Shader;
- VulkanInitializer (handles vulkan-specific initializations);
- DeviceManager (finds and selects graphics physical device, creates logical device);
- VertexBufferLayout (specifies layout of data in vertex buffer);
- Window (Application Window);
- SwapChain (handles frame buffers and data display sync);
- Renderer

## Renderer

- Contact point with Application;
- Handles Buffer, Texture, and Shader creation;
- Exposes Graphics API to Application (beginDraw, endDraw, settings commands);
