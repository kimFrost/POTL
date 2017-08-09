import { Component, OnInit, Input } from '@angular/core';

import { Structure } from '../structure';
import { Tile } from '../tile';
import { Provider } from '../provider';
import { TileManagerService } from '../tile-manager.service';

@Component({
  selector: 'structure',
  templateUrl: './structure.component.html',
  styleUrls: ['./structure.component.scss']
})
export class StructureComponent implements OnInit {

  @Input()
  y: number;

  @Input()
  x: number;

  @Input()
  structure: Structure

  private tile: Tile;
  private tilesInRange: Tile[];

  //transclude = true;

  constructor(private tileManagerService: TileManagerService) {
    this.tilesInRange = [];
  }

  ngOnInit() {
    this.tile = this.tileManagerService.getTile(this.x, this.y);
    if (this.tile) {
      this.tilesInRange = this.tileManagerService.getTilesInRange(this.tile, 5);
      if (this.structure) {
        for (let provider of this.structure.providers) {
          for (let tile of this.tilesInRange) {
            if (tile) {
              tile.providers.push(provider);
            }
          }
        }
      }
    }
  }

}
