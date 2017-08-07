import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { TileComponent } from './tile/tile.component';
import { TileManagerComponent } from './tile-manager/tile-manager.component';
import { ToolsComponent } from './tools/tools.component';
import { StructureComponent } from './structure/structure.component';
import { ProviderComponent } from './provider/provider.component';

@NgModule({
  declarations: [
    AppComponent,
    TileComponent,
    TileManagerComponent,
    ToolsComponent,
    StructureComponent,
    ProviderComponent
  ],
  imports: [
    BrowserModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
