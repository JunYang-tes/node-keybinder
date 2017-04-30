# Dependencies

+ GTK+ 3.0 (keybinder-3.0)
+ gobject-introspection
+ libkeybinder-3.0

# Usage
```
const keybinder = require("keybinder");

keybinder.on("<Ctrl>B", () => {
  console.log("Ctrl+B")
})
```