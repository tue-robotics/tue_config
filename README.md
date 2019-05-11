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

# Old
## TO DO

* Support newest YAML lib API

## Ideas

### Add readMap

Same behavior / interface as reading Arrays, but then for iterating through map
