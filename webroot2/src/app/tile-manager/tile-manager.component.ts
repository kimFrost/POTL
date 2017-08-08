import { Component, OnInit } from '@angular/core';


//import { HeroSearchService } from './hero-search.service';
import { Tile } from '../tile';
import { Provider } from '../provider';

@Component({
  selector: 'tile-manager',
  templateUrl: './tile-manager.component.html',
  styleUrls: ['./tile-manager.component.css']
})
export class TileManagerComponent implements OnInit {

  public tileSize = 30;
  public tiles: Tile[];
  private numOfRows = 30;
  private numOfColumns = 30;
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
    let tiles = this.getTilesInRange(this.tiles[90], 5);
    for (let tile of tiles) {
      if (tile) {
        tile.providers.push(new Provider('wheat', 1));
      }
    }
    tiles = this.getTilesInRange(this.tiles[97], 5);
    for (let tile of tiles) {
      if (tile) {
        tile.providers.push(new Provider('wheat', 1));
      }
    }
  }

  ngOnInit() { }

  private _enrichTiles() {
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

  public getTile(x: number, y: number): Tile {
    let index = y * this.numOfColumns + x;
    return this.tiles[index];
  }

  public getTilesInRange(startTile: Tile, range: number): Tile[] {
    let visited: Tile[] = [startTile];
    if (startTile) {
      //let frontiers:Tile[][] = [];
      let frontiers: Array<Array<Tile>> = [[startTile]];
      for (let k = 1; k <= range; k++) {
        frontiers.push([]);
        let frontier: Tile[] = frontiers[k - 1];
        for (let tile of frontier) {
          if (tile) {
            for (let neighbour of tile.neighbours) {
              if (neighbour && visited.indexOf(neighbour) === -1) {
                visited.push(neighbour);
                frontiers[k].push(neighbour);
              }
            }
          }
        }
      }
    }
    return visited;
  }

}
