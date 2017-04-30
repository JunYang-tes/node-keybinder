{
  "targets": [
    {
      "target_name": "keybinder",
      "sources": [
        "src/binder.cc"
      ],
       "conditions":[
        ['OS=="linux"',{
            'cflags':[
              '<!@(pkg-config --cflags keybinder-3.0)'
            ],
            'cflags_cc': ['-fexceptions'],
            'cflags_cc!': [ '-fno-rtti' ],
            'ldflags':[
              '<!@(pkg-config --libs keybinder-3.0)'
            ]
          }
        ]
      ],
      "libraries":[
        '<!@(pkg-config --libs gtkmm-3.0 keybinder-3.0)'
      ],
     }
  ]
}
