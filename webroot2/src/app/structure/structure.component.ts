import { Component, OnInit, Input } from '@angular/core';

@Component({
  selector: 'structure',
  templateUrl: './structure.component.html',
  styleUrls: ['./structure.component.css']
})
export class StructureComponent implements OnInit {

  @Input()
  y: number;

  @Input()
  x: number;

  //transclude = true;

  constructor() { }

  ngOnInit() {
    
  }

}
