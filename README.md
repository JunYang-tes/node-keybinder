# Dependencies
+ node-gyp
+ GTK+ 3.0 (keybinder-3.0)
+ gobject-introspection
+ libkeybinder-3.0

# Usage
Using npm or yarn to install
```
yarn  add node-keybinder
```
or
```
npm install node-keybinder
```

```
const keybinder = require("keybinder");

keybinder.on("<Ctrl>B", () => {
  console.log("Ctrl+B")
})
```