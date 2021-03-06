import { BrowserModule } from '@angular/platform-browser';
import { RouterModule } from '@angular/router';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { TileComponent } from './tile/tile.component';
import { TileManagerComponent } from './tile-manager/tile-manager.component';
import { ToolsComponent } from './tools/tools.component';
import { StructureComponent } from './structure/structure.component';
import { ProviderComponent } from './provider/provider.component';
import { OverlaySwitcherComponent } from './overlay-switcher/overlay-switcher.component';

import { OverlayService } from './overlay.service';
import { TileManagerService } from './tile-manager.service';

@NgModule({
  declarations: [
    AppComponent,
    TileComponent,
    TileManagerComponent,
    ToolsComponent,
    StructureComponent,
    ProviderComponent,
    OverlaySwitcherComponent
  ],
  imports: [
    BrowserModule
  ],
  providers: [OverlayService, TileManagerService],
  bootstrap: [AppComponent]
})
export class AppModule { }
