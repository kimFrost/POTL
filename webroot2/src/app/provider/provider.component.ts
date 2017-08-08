import { Component, OnInit, Input } from '@angular/core';

import { StructureComponent } from '../structure/structure.component';

@Component({
  selector: 'provider',
  templateUrl: './provider.component.html',
  styleUrls: ['./provider.component.css']
})
export class ProviderComponent implements OnInit {

  @Input()
  structure: StructureComponent;

  constructor() { }

  ngOnInit() {
  }

}
