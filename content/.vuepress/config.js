module.exports = {
  base: '/',
  themeConfig: {
    repo: 'particle-iot/spectra-workshop-2020',
    docsDir: 'content',
    editLinks: true,
    editLinkText: 'Help us improve this page!',
    nav: [
      { text: 'Home', link: '/' },
      { text: 'Team', link: 'http://particle.io' }
    ],
    sidebar: [
      '/docs/',
      ['/docs/ch1', 'Lab 1: Getting your Particle Argon online'],
      [
        '/docs/ch2',
        'Lab 2: Working with Particle Workbench & Primitives'
      ],
      [
        '/docs/ch3',
        'Lab 3: Exploring BLE & Advanced Device OS Features'
      ],
      [
        '/docs/ch4', 
        'Lab 4: Exploring Particle SDKs & Integrations'
      ],
      ['/docs/extra', 'Extra: Going Even Further!']
    ]
  },
  title: 'Particle Accelerate Workshop',
  description:
    'Workshops designed to teach the basics of IoT development with the Particle ecosystem & the Particle Argon'
};
