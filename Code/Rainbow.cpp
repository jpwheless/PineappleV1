void Rainbow::init(RGBinterface *rgb) {
	this->rgb = rgb;
	active = false;
	timer = 0;
	i = 0;
}

void Rainbow::cycle() {
	if (active) {
		if (timer >= 4000) {
			i++;
			if (i > 255) {
				i = 0;
				currentColor++;
				if (currentColor > 5) currentColor = 0;
			}
			timer = 0;
		}
		switch (currentColor) {
			case 0:
				rgb->color(i, 0, 0);
				break;
			case 1:
				rgb->color(255, i, 0);
				break;
			case 2:
				rgb->color((255-i), 255, 0);
				break;
			case 3:
				rgb->color(0, 255, i);
				break;
			case 4:
				rgb->color(0, (255-i), 255);
				break;
			case 5:
				rgb->color(0, 0, (255-i));
				break;
		}
	}
}

void Rainbow::on() {
	active = true;
	currentColor = 0;
	timer = 0;
	i = 0;
}

void Rainbow::off() {
	active = false;
	rgb->color(0, 0, 0);
}