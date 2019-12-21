{
  "name": "HoW Campaign",
  "author": "Gremlin Interactive",
  "version": "v1.0",
  "isVisible": true,
  "dependencies": [
    "base"
  ],
  "scenarios": [
    {
      "name": "default",
      "maps": [
        {
          "name": "camp",
          "timer": "99",
          "opponents": []
        },
        {
          "name": "estu",
          "timer": "99",
          "opponents": [
            {
              "className": "grunt"
            },
            {
              "className": "grunt"
            },
            {
              "className": "grunt"
            },
            {
              "className": "grunt"
            }
          ]
        }
      ]
    }
  ]
}