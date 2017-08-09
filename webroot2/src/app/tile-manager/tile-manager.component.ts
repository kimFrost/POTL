import { Component, OnInit } from '@angular/core';

import { Tile } from '../tile';
import { Provider } from '../provider';
import { Structure } from '../structure';
import { TileManagerService } from '../tile-manager.service';

@Component({
  selector: 'tile-manager',
  templateUrl: './tile-manager.component.html',
  styleUrls: ['./tile-manager.component.scss']
})
export class TileManagerComponent implements OnInit {

  public tileSize = 30;
  public tiles: Tile[];

  constructor(private tileManagerService: TileManagerService) { }

  ngOnInit() {
    this.tileSize = this.tileManagerService.tileSize;
    this.tiles = this.tileManagerService.tiles;
  }

}
