import React from 'react'
import { css } from 'glamor'
import githubImg from '../static/img/github.svg'
import tumblrImg from '../static/img/tumblr.svg'

const links = [
	{
		img: githubImg,
		url: 'https://github.com/doublejosh/FortuneTeller',
		label: 'GitHub',
	},
	{
		img: tumblrImg,
		url: 'https://doublejosh.com',
		label: 'Blog',
	},
]

export default () => (
	<section>
		<ul
			{...css({
				listStyle: 'none',
				margin: '5rem 0 0 0',
				padding: '0 0 10rem 0',
				textAlign: 'center',
			})}>
			{links &&
				links.map(link => (
					<li
						key={link.url}
						{...css({
							display: 'inline',
							marginLeft: '2rem',
							'&:nth-child(1)': {
								marginLeft: 0,
							},
						})}>
						<a href={link.url} target="_blank" rel="noopener noreferrer">
							<img
								src={link.img}
								alt={link.label}
								{...css({
									width: '40px',
								})}
							/>
						</a>
					</li>
				))}
		</ul>
	</section>
)
