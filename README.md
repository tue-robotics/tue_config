# tue_config

## Special keys
- **include**: relative or absolute file path which is inserted at that level. Always place the include tag at the top of
  a node. This allows content from the included yaml file to be overruled by adding the same key at the correct child
  level. This only works for mappings, not for sequences. As a new item will be created.
### Examples
#### Example 1 (include)
**box.yaml**
```yaml
box:
  size:
      x: 1
      y: 1
      z: 1
```
**composite.yaml**
```yaml
composite:
  - include: box.yaml
    pose: {x: 0, y: 0, z: 0}
  - include: box.yaml
    pose: {x: 0, y: 0, z: 1}
    box:
      size:
        z: 2
```
**parsed composite.yaml** (Overruling of box size works correct)
```yaml
composite:
- box:
    size:
      x: 1
      y: 1
      z: 1
  pose: {x: 0, y: 0, z: 0}
- box:
    size:
      x: 1
      y: 1
      z: 2
  pose: {x: 0, y: 0, z: 1}
```
**composite2.yaml**
```yaml
composite:
  - include: box.yaml
    pose: {x: 0, y: 0, z: 0}
  - box:
      size:
        z: 2
    include: box.yaml
    pose: {x: 0, y: 0, z: 1}
```
**parsed composite2.yaml** (Overruling doesn't work, if defined before **include**)
```yaml
composite:
- box:
    size:
      x: 1
      y: 1
      z: 1
  pose: {x: 0, y: 0, z: 0}
- box:
    size:
      x: 1
      y: 1
      z: 1
  pose: {x: 0, y: 0, z: 1}
```
#### Example 2 (include)
**subgroup.yaml**
```yaml
persons:
  - name: David
    age: 37
  - name: Jacob
    age: 29
```
**group.yaml**
```yaml
include: subgroup.yaml
persons:
  - name: David
    age: 43
```
**parsed group.yaml** (Overruling of sequence item isn't possible. New item is created)
```yaml
persons:
- age: 37
  name: David
- age: 29
  name: Jacob
- age: 43
  name: David
```

## Special values
Special values are resolved on parsing. The resolve function all have the same structure: `$(CMD args)`. The following
commands are available:
- **rospkg** (PKG_NAME): **rospkg** resolves to the root folder of the ros package. The only argument is required and is
  the name of the ros package you want to find. If the package isn't found, parsing fails.
- **env** (ENV_VARIABLE [DEFAULT]): **env** resolves to an environment variable. If the environment variable isn't
  available and a default value is provided, this value will be returned. If no default value is provided, parsing fails.
- **file** (FILE_NAME): **file** resolves to an absolute/relative file path. This depends on if the root file was an
  absolute or relative path. This function never returns a false. No file existence check is done.

# Old
## TO DO

* Support newest YAML lib API

## Ideas

### Add readMap

Same behavior / interface as reading Arrays, but then for iterating through map
