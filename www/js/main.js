(function() {
  $('#muter').click(function() {
    var sound = document.getElementById('sound');
    sound.muted = !sound.muted;

    if (sound.muted) {
      $('#muter i').removeClass('fa-volume-up').addClass('fa-volume-off');
    }
    else {
      $('#muter i').removeClass('fa-volume-off').addClass('fa-volume-up');
    }
  });

  $('.savoirplusswitch').click(function() {
    var body = $('body');

    if (body.hasClass('savoirplus')) {
      body.removeClass('savoirplus');
      body.removeClass('extended');
    }
    else {
      body.addClass('savoirplus');
      body.addClass('extended');
    }
  });

  //
  // Let's code some fireflies !
  //

  const LUCIOLE_SIZE = 50;
  const LUCIOLE_SPEED = 10;
  const LUCIOLE_VIEW = 10;
  const LUCIOLE_FLASH_LENGTH = 30;
  const LUCIOLE_DELAY = 65;
  const LUCIOLE_PER_CONTAINER = 10;

  const ATAN_10 = Math.atan(10);
  const ATAN_M_10 = Math.atan(-10);
  const ATAN_DIFF = ATAN_10 - ATAN_M_10;

  var atans = new Array();

  for (var i = 0; i <= 20; i++) {
    atans.push(Math.atan(i - 10));
  }

  var sign = Math.sign;

  if (!sign) {
    sign = function(x) {
      return x < 0 ? -1 : (x == 0 ? 0 : 1);
    };
  }

  function atanScale(x) {
    x = (5 - sign(x) * x) * 2;

    var s = Math.min(ATAN_10, Math.max(ATAN_M_10, atans[x+10]));
    return (s - ATAN_M_10) / ATAN_DIFF;
  }

  function LuciolesContainer(id) {
    this.svg = d3.select(id).append('svg');
    this.containerId = id;

    var defs     = this.svg.append("defs")
      , grad     = defs.append('radialGradient');

    grad.attr('id', 'LucioleGradient');
    grad.append('stop')
      .attr('offset', '0%')
      .attr('stop-color', 'rgba(194, 196, 13, 0.9)');
    grad.append('stop')
      .attr('offset', '100%')
      .attr('stop-color', 'rgba(194, 196, 13, 0.0)');

    this.luciolesContainer = this.svg.append('g')

    this.width    = $(id + ' svg').width()
    this.height   = $(id + ' svg').height();

    this.lucioles = new Array();

    this.counter = 0;
  }

  LuciolesContainer.prototype.resize = function() {
    this.width    = $(this.containerId + ' svg').width()
    this.height   = $(this.containerId + ' svg').height();
  };

  LuciolesContainer.prototype.addLuciole = function() {
    var l = new Luciole(this, this.svg.append('rect'));
    this.lucioles.push(l);
  };

  LuciolesContainer.prototype.step = function() {
    this.lucioles.forEach(function(l, i, a) {
      l.next_step = LUCIOLE_VIEW * l.step;
      l.move();

      if (this.counter == 0) {
        for (var j = 0; j < LUCIOLE_VIEW; j++) {
          l.next_step += a[(i+j)%a.length].step;
        }
      }

      l.next_step /= 2 * LUCIOLE_VIEW;
    });

    if (this.counter == 0) {
      this.lucioles.forEach(function(l, i) {
        l.step = l.step - Math.ceil((l.step - l.next_step) * 0.2);
      });
    }

    this.counter = (this.counter + 1) % 100;
  };

  function Luciole(root, e) {
    this.root = root;

    this.container = e;
    this.container
      .attr('width',  LUCIOLE_SIZE)
      .attr('height', LUCIOLE_SIZE)
      .attr('fill', 'url(#LucioleGradient)');

    this.x = Math.random() * this.root.width;
    this.y = Math.random() * this.root.height;

    this.dx = Math.random() * LUCIOLE_SPEED * 2 - LUCIOLE_SPEED;
    this.dy = Math.random() * LUCIOLE_SPEED * 2 - LUCIOLE_SPEED;

    this.step = Math.ceil(Math.random() * 100);
    this.next_step = this.step;

    this.is_flashing = this.step == 0;
    this.flash_step = 0;

    this.container.style('opacity', 0);
  }

  Luciole.prototype.move = function() {
    this.x = this.x + this.dx;
    this.y = this.y + this.dy;

    if (this.x < 0) {
      this.x = 0;
      this.dx *= -1;
    }
    else if (this.x >= this.root.width) {
      this.x = this.root.width - 1;
      this.dx *= -1;
    }

    if (this.y < 0) {
      this.y = 0;
      this.dy *= -1;
    }
    else if (this.y >= this.root.height) {
      this.y = this.root.height - 1;
      this.dy *= -1;
    }

    this.dx = Math.max(-LUCIOLE_SPEED,
      Math.min(LUCIOLE_SPEED, this.dx + Math.random() * 4 - 2));
    this.dy = Math.max(-LUCIOLE_SPEED,
      Math.min(LUCIOLE_SPEED, this.dy + Math.random() * 4 - 2));

    this.step = (this.step + 1) % 100;

    if (this.step == 0) {
      this.is_flashing = true;
      this.flash_step = 0;
    }
    else if (this.is_flashing) {
      this.container
        .attr('x', this.x)
        .attr('y', this.y);

      this.container.style('opacity', atanScale(20 * this.flash_step / LUCIOLE_FLASH_LENGTH - 10));
      this.flash_step++;

      if (this.flash_step > LUCIOLE_FLASH_LENGTH) {
        this.is_flashing = false;
        this.container.style('opacity', 0);
      }
    }
  };

  var containers = new Array();
  containers.push(new LuciolesContainer('#lucioles-container-1'));
  containers.push(new LuciolesContainer('#lucioles-container-2'));
  containers.push(new LuciolesContainer('#lucioles-container-3'));

  containers.forEach(function(c) {
    for(var i = 0; i < LUCIOLE_PER_CONTAINER; i++)
      c.addLuciole();
  });

  window.addEventListener('resize', function() {
    containers.forEach(function(c) {
      c.resize();
    });
  });

  /*var date_flash = 0
    , date_cont = d3.select('#date');*/

  setInterval(function() {
    containers.forEach(function(c) {
      c.step();
    });

    /*date_cont.style('opacity', atanScale(20 * date_flash / LUCIOLE_FLASH_LENGTH - 10));
    date_flash = (date_flash + 1) % LUCIOLE_FLASH_LENGTH;*/
  }, LUCIOLE_DELAY);
})();
