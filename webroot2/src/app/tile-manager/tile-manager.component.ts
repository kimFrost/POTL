import { Component, OnInit } from '@angular/core';


//import { HeroSearchService } from './hero-search.service';
import { Tile } from '../tile';

@Component({
  selector: 'tile-manager',
  templateUrl: './tile-manager.component.html',
  styleUrls: ['./tile-manager.component.css']
})
export class TileManagerComponent implements OnInit {

  public tileSize = 50;
  public tiles: Tile[];
  private numOfRows = 15;
  private numOfColumns = 20;
  //private searchTerms = new Subject<string>();

  constructor() {
    this.tiles = [];
    for (let y = 0; y < this.numOfRows; y++) {
      for (let x = 0; x < this.numOfColumns; x++) {
        let tile = new Tile(x, y);
        this.tiles.push(tile);
      }
    }
    this._enrichTiles();
  }

  ngOnInit() {
    for (let tile of this.tiles) {
      if (tile) {
        let neighbours = [
          this.getTile(tile.x, tile.y - 1),
          this.getTile(tile.x + 1, tile.y),
          this.getTile(tile.x, tile.y + 1),
          this.getTile(tile.x - 1, tile.y)
        ];
        tile.neighbours = neighbours;
      }
    }
  }

  private _enrichTiles() {

  }

  public getTile(x: number, y: number): Tile {
    let index = y * this.numOfColumns + x;
    return this.tiles[index];
  }

}
