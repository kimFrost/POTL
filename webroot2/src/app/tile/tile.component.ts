import { Component, OnInit, OnDestroy, Input, Output } from '@angular/core';

import { Subscription } from 'rxjs/Subscription';

import { Tile } from '../tile';
import { Provider } from '../provider';
import { Structure } from '../structure';
import { OverlayService } from '../overlay.service';

@Component({
  selector: 'tile',
  templateUrl: './tile.component.html',
  styleUrls: ['./tile.component.scss']
})
export class TileComponent implements OnInit {

  @Input()
  tile: Tile;

  public activeProvider: Provider;

  private subscription: Subscription;

  constructor(private overlayService: OverlayService) {}

  ngOnInit() {
    if (this.tile) {
      if (Math.random() > 0.5) {
        this.tile.providers.push(new Provider('lumber', 1));
        for (let neighbour of this.tile.neighbours) {
          if (neighbour) {
            neighbour.providers.push(new Provider('lumber', 1));
          }
        }
      }
    }

    this.subscription = this.overlayService.getProviderProfile().subscribe(provider => {
      this.activeProvider = provider;
    }, error => console.log('Could not get provider profile'));
  }

  ngOnDestroy() {
    this.subscription.unsubscribe();
  }

}
