import { Component, OnInit } from '@angular/core';

import { Provider } from '../provider';
import { OverlayService } from '../overlay.service';

@Component({
  selector: 'overlay-switcher',
  templateUrl: './overlay-switcher.component.html',
  styleUrls: ['./overlay-switcher.component.css']
})
export class OverlaySwitcherComponent implements OnInit {

  public options: Provider[];
  public currentOption: Provider;

  constructor(private overlayService:OverlayService) {
    this.options = [];
    this.options.push(new Provider('none', 0));
    this.options.push(new Provider('lumber', 0));
    this.options.push(new Provider('wheat', 0));
    this.options.push(new Provider('flour', 0));
    this.options.push(new Provider('bread', 0));
  }

  ngOnInit() {
  }

  public setOption = (option): void => {
    if (this.currentOption === option) {
      return;
    }
    this.currentOption = option;
    this.overlayService.setProviderProfile(this.currentOption)
  }

}
