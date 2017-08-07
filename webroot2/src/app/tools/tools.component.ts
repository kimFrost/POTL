import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'tools',
  templateUrl: './tools.component.html',
  styleUrls: ['./tools.component.css']
})
export class ToolsComponent implements OnInit {

  public tools = [
    {
      id: 'paint',
      options: [
        'forest',
        'grass',
        'water'
      ]
    },
    {
      id: 'place',
      options: [
        'house'
      ]
    }
  ];

  constructor() { }

  ngOnInit() {
  }

  setTool(tool) {

  }

}
